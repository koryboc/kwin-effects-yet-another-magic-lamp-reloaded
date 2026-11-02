/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 * Copyright (C) 2026 Roy Bock <dev@si13n7.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// Own
#include "Model.h"
#include "kwincompat.h"

#include <cmath>

static inline std::chrono::milliseconds durationFraction(std::chrono::milliseconds duration, qreal fraction)
{
    return std::chrono::milliseconds(qMax(qRound(duration.count() * fraction), 1));
}

Model::Model(KWin::EffectWindow* window)
    : m_window(window)
{
}

// Mirrors KDE's own Magic Lamp logic exactly:
// 1. Find panel via intersects() — Plasma sometimes publishes icon geometry
//    slightly larger than the panel, so contains() misses it.
// 2. Use clientArea(ScreenArea, window) — the WINDOW's screen center — to
//    determine direction. This correctly handles cross-screen icons: even if
//    the icon is on a different screen, comparing to the window's screen
//    center always gives the right direction.
static Direction realizeDirection(const KWin::EffectWindow* window, const QRectF& iconRect)
{
    KWin::EffectWindow* panel = nullptr;
    for (KWin::EffectWindow* candidate : KWin::effects->stackingOrder()) {
        if (!candidate->isDock()) {
            continue;
        }
        if (candidate->frameGeometry().intersects(iconRect)) {
            panel = candidate;
            break;
        }
    }

    if (panel) {
        const QRectF windowScreen = KWin::effects->clientArea(KWin::ScreenArea, window);
        if (panel->frameGeometry().width() >= panel->frameGeometry().height()) {
            return iconRect.center().y() <= windowScreen.center().y()
                ? Direction::Top
                : Direction::Bottom;
        } else {
            return iconRect.center().x() <= windowScreen.center().x()
                ? Direction::Left
                : Direction::Right;
        }
    }

    // No panel found — fall back to closest screen edge (KDE uses ScreenArea intersect here).
    KWin::LogicalOutput* screen = KWin::effects->screenAt(iconRect.center().toPoint());
    const QRectF screenRect = KWinCompat::fullScreenArea(KWin::effects, screen);
    const QPointF c = iconRect.center();
    const qreal l = qAbs(c.x() - screenRect.left());
    const qreal t = qAbs(c.y() - screenRect.top());
    const qreal r = qAbs(screenRect.right() - c.x());
    const qreal b = qAbs(screenRect.bottom() - c.y());
    const qreal m = qMin(qMin(l, t), qMin(r, b));
    if (qFuzzyCompare(l, m))
        return Direction::Left;
    if (qFuzzyCompare(t, m))
        return Direction::Top;
    if (qFuzzyCompare(r, m))
        return Direction::Right;
    return Direction::Bottom;
}

void Model::start(AnimationKind kind)
{
    // A reversal while flying just turns around; a new request during the
    // final wobble starts over.
    if (m_timeLine.running() && m_stage != AnimationStage::Settle) {
        m_kind = kind;
        m_timeLine.toggleDirection();
        return;
    }
    m_kind = kind;
    m_done = false;

    if (!m_iconGeometry.isValid()) {
        m_iconGeometry = m_window->iconGeometry();
    }
    m_direction = realizeDirection(m_window, m_iconGeometry);
    m_bumpDistance = computeBumpDistance();
    m_shapeFactor = computeShapeFactor();

    switch (m_kind) {
    case AnimationKind::Minimize:
        if (m_bumpDistance != 0) {
            startBump(KWin::TimeLine::Forward);
        } else {
            startTravel(KWin::TimeLine::Forward, false);
        }
        break;

    case AnimationKind::Unminimize:
        startTravel(KWin::TimeLine::Backward, true);
        break;

    default:
        Q_UNREACHABLE();
    }
}

void Model::startBump(KWin::TimeLine::Direction direction)
{
    m_stage = AnimationStage::Bump;
    m_timeLine.reset();
    m_timeLine.setDirection(direction);
    m_timeLine.setDuration(m_parameters.bumpDuration);
    m_timeLine.setEasingCurve(QEasingCurve::InOutSine);
    m_clip = false;
}

void Model::startTravel(KWin::TimeLine::Direction direction, bool coupledBump)
{
    // Stretch and squash run on one timeline so the motion curve applies to
    // the whole flight. m_split marks where stretching hands over to squashing.
    const auto stretch = durationFraction(coupledBump ? m_parameters.unminimizeStretchDuration
                                                      : m_parameters.stretchDuration,
        m_shapeFactor);
    const auto squash = std::max(m_parameters.squashDuration, std::chrono::milliseconds(1));
    m_split = qreal(stretch.count()) / qreal(stretch.count() + squash.count());
    m_coupledBump = coupledBump;
    m_stage = AnimationStage::Travel;
    m_timeLine.reset();
    m_timeLine.setDirection(direction);
    m_timeLine.setDuration(stretch + squash);
    m_timeLine.setEasingCurve(QEasingCurve::Linear); // the motion curve is applied in travelProgress()
    updateClip();
}

void Model::startSettleOrFinish()
{
    if (m_parameters.spring <= 0.0 || m_parameters.settleDuration.count() <= 0) {
        m_done = true;
        return;
    }
    m_stage = AnimationStage::Settle;
    m_timeLine.reset();
    m_timeLine.setDirection(KWin::TimeLine::Forward);
    m_timeLine.setDuration(m_parameters.settleDuration);
    m_timeLine.setEasingCurve(QEasingCurve::Linear);
    m_clip = false;
}

qreal Model::travelProgress() const
{
    // "Anticipate" (InBack) dips below zero: the window first pulls away from
    // the icon, and on the way back it overshoots its own size a little.
    return qBound(-0.15, m_parameters.motionCurve.valueForProgress(m_timeLine.value()), 1.0);
}

void Model::updateClip()
{
    if (m_stage != AnimationStage::Travel) {
        m_clip = false;
        return;
    }
    // Like the original stages: Stretch1 and Squash clip at the icon edge,
    // the unminimize stretch (bump returning at the same time) does not.
    m_clip = !m_coupledBump || travelProgress() >= m_split;
}

void Model::advance(std::chrono::milliseconds presentTime)
{
    if (m_done) {
        return;
    }
    m_timeLine.advance(presentTime);
    updateClip();
    if (!m_timeLine.done()) {
        return;
    }

    const bool forward = m_timeLine.direction() == KWin::TimeLine::Forward;
    switch (m_stage) {
    case AnimationStage::Bump:
        if (forward) {
            startTravel(KWin::TimeLine::Forward, false); // raised: now fly into the icon
        } else {
            startSettleOrFinish(); // came back down: fully restored
        }
        return;

    case AnimationStage::Travel:
        if (forward) {
            m_done = true; // inside the icon
        } else if (!m_coupledBump && m_bumpDistance != 0) {
            startBump(KWin::TimeLine::Backward); // reversed minimize: lower it again
        } else {
            startSettleOrFinish();
        }
        return;

    case AnimationStage::Settle:
        m_done = true;
        return;
    }
}

bool Model::done() const
{
    return m_done;
}

struct TransformParameters {
    QEasingCurve shapeCurve;
    Direction direction;
    QRectF iconRect;
    qreal stretchProgress;
    qreal squashProgress;
    qreal bumpProgress;
    qreal bumpDistance;
    // sideways extras, in pixels: arc of the neck and cloth ripple
    qreal bend = 0.0;
    qreal waveAmplitude = 0.0;
    qreal waveLength = 1.0;
    qreal wavePhase = 0.0;
};

// Sideways offset of an edge that has moved `scale` (0..1) of the way to the
// icon and sits at `along` on the motion axis. sin(pi * scale) keeps the
// window body and the very tip at the icon in place; only the neck moves.
static inline qreal sideShift(const TransformParameters& params, qreal scale, qreal along)
{
    const qreal envelope = std::sin(M_PI * qBound(0.0, scale, 1.0));
    if (envelope <= 0.0) {
        return 0.0;
    }
    qreal shift = params.bend;
    if (params.waveAmplitude > 0.0) {
        shift += params.waveAmplitude * std::sin(2.0 * M_PI * along / params.waveLength - params.wavePhase);
    }
    return shift * envelope;
}

static inline qreal interpolate(qreal from, qreal to, qreal t)
{
    return from * (1.0 - t) + to * t;
}

static void transformQuadsLeft(
    const KWin::EffectWindow* window,
    const TransformParameters& params,
    KWin::WindowQuadList& quads)
{
    const QRectF iconRect = params.iconRect;
    const QRectF windowRect = window->frameGeometry();

    const qreal distance = windowRect.right() - iconRect.right() + params.bumpDistance;

    for (int i = 0; i < quads.count(); ++i) {
        KWin::WindowQuad& quad = quads[i];

        const qreal leftOffset = quad[0].x() - interpolate(0.0, distance, params.squashProgress);
        const qreal rightOffset = quad[2].x() - interpolate(0.0, distance, params.squashProgress);

        const qreal leftScale = params.stretchProgress * params.shapeCurve.valueForProgress((windowRect.width() - leftOffset) / distance);
        const qreal rightScale = params.stretchProgress * params.shapeCurve.valueForProgress((windowRect.width() - rightOffset) / distance);

        const qreal targetTopLeftY = iconRect.y() + iconRect.height() * quad[0].y() / windowRect.height();
        const qreal targetTopRightY = iconRect.y() + iconRect.height() * quad[1].y() / windowRect.height();
        const qreal targetBottomRightY = iconRect.y() + iconRect.height() * quad[2].y() / windowRect.height();
        const qreal targetBottomLeftY = iconRect.y() + iconRect.height() * quad[3].y() / windowRect.height();

        quad[0].setY(quad[0].y() + leftScale * (targetTopLeftY - (windowRect.y() + quad[0].y())));
        quad[3].setY(quad[3].y() + leftScale * (targetBottomLeftY - (windowRect.y() + quad[3].y())));
        quad[1].setY(quad[1].y() + rightScale * (targetTopRightY - (windowRect.y() + quad[1].y())));
        quad[2].setY(quad[2].y() + rightScale * (targetBottomRightY - (windowRect.y() + quad[2].y())));

        const qreal targetLeftOffset = leftOffset + params.bumpDistance * params.bumpProgress;
        const qreal targetRightOffset = rightOffset + params.bumpDistance * params.bumpProgress;

        quad[0].setX(targetLeftOffset);
        quad[3].setX(targetLeftOffset);
        quad[1].setX(targetRightOffset);
        quad[2].setX(targetRightOffset);

        const qreal leftShift = sideShift(params, leftScale, targetLeftOffset);
        const qreal rightShift = sideShift(params, rightScale, targetRightOffset);
        quad[0].setY(quad[0].y() + leftShift);
        quad[3].setY(quad[3].y() + leftShift);
        quad[1].setY(quad[1].y() + rightShift);
        quad[2].setY(quad[2].y() + rightShift);
    }
}

static void transformQuadsTop(
    const KWin::EffectWindow* window,
    const TransformParameters& params,
    KWin::WindowQuadList& quads)
{
    const QRectF iconRect = params.iconRect;
    const QRectF windowRect = window->frameGeometry();

    const qreal distance = windowRect.bottom() - iconRect.bottom() + params.bumpDistance;

    for (int i = 0; i < quads.count(); ++i) {
        KWin::WindowQuad& quad = quads[i];

        const qreal topOffset = quad[0].y() - interpolate(0.0, distance, params.squashProgress);
        const qreal bottomOffset = quad[2].y() - interpolate(0.0, distance, params.squashProgress);

        const qreal topScale = params.stretchProgress * params.shapeCurve.valueForProgress((windowRect.height() - topOffset) / distance);
        const qreal bottomScale = params.stretchProgress * params.shapeCurve.valueForProgress((windowRect.height() - bottomOffset) / distance);

        const qreal targetTopLeftX = iconRect.x() + iconRect.width() * quad[0].x() / windowRect.width();
        const qreal targetTopRightX = iconRect.x() + iconRect.width() * quad[1].x() / windowRect.width();
        const qreal targetBottomRightX = iconRect.x() + iconRect.width() * quad[2].x() / windowRect.width();
        const qreal targetBottomLeftX = iconRect.x() + iconRect.width() * quad[3].x() / windowRect.width();

        quad[0].setX(quad[0].x() + topScale * (targetTopLeftX - (windowRect.x() + quad[0].x())));
        quad[1].setX(quad[1].x() + topScale * (targetTopRightX - (windowRect.x() + quad[1].x())));
        quad[2].setX(quad[2].x() + bottomScale * (targetBottomRightX - (windowRect.x() + quad[2].x())));
        quad[3].setX(quad[3].x() + bottomScale * (targetBottomLeftX - (windowRect.x() + quad[3].x())));

        const qreal targetTopOffset = topOffset + params.bumpDistance * params.bumpProgress;
        const qreal targetBottomOffset = bottomOffset + params.bumpDistance * params.bumpProgress;

        quad[0].setY(targetTopOffset);
        quad[1].setY(targetTopOffset);
        quad[2].setY(targetBottomOffset);
        quad[3].setY(targetBottomOffset);

        const qreal topShift = sideShift(params, topScale, targetTopOffset);
        const qreal bottomShift = sideShift(params, bottomScale, targetBottomOffset);
        quad[0].setX(quad[0].x() + topShift);
        quad[1].setX(quad[1].x() + topShift);
        quad[2].setX(quad[2].x() + bottomShift);
        quad[3].setX(quad[3].x() + bottomShift);
    }
}

static void transformQuadsRight(
    const KWin::EffectWindow* window,
    const TransformParameters& params,
    KWin::WindowQuadList& quads)
{
    const QRectF iconRect = params.iconRect;
    const QRectF windowRect = window->frameGeometry();

    const qreal distance = iconRect.left() - windowRect.left() + params.bumpDistance;

    for (int i = 0; i < quads.count(); ++i) {
        KWin::WindowQuad& quad = quads[i];

        const qreal leftOffset = quad[0].x() + interpolate(0.0, distance, params.squashProgress);
        const qreal rightOffset = quad[2].x() + interpolate(0.0, distance, params.squashProgress);

        const qreal leftScale = params.stretchProgress * params.shapeCurve.valueForProgress(leftOffset / distance);
        const qreal rightScale = params.stretchProgress * params.shapeCurve.valueForProgress(rightOffset / distance);

        const qreal targetTopLeftY = iconRect.y() + iconRect.height() * quad[0].y() / windowRect.height();
        const qreal targetTopRightY = iconRect.y() + iconRect.height() * quad[1].y() / windowRect.height();
        const qreal targetBottomRightY = iconRect.y() + iconRect.height() * quad[2].y() / windowRect.height();
        const qreal targetBottomLeftY = iconRect.y() + iconRect.height() * quad[3].y() / windowRect.height();

        quad[0].setY(quad[0].y() + leftScale * (targetTopLeftY - (windowRect.y() + quad[0].y())));
        quad[3].setY(quad[3].y() + leftScale * (targetBottomLeftY - (windowRect.y() + quad[3].y())));
        quad[1].setY(quad[1].y() + rightScale * (targetTopRightY - (windowRect.y() + quad[1].y())));
        quad[2].setY(quad[2].y() + rightScale * (targetBottomRightY - (windowRect.y() + quad[2].y())));

        const qreal targetLeftOffset = leftOffset - params.bumpDistance * params.bumpProgress;
        const qreal targetRightOffset = rightOffset - params.bumpDistance * params.bumpProgress;

        quad[0].setX(targetLeftOffset);
        quad[3].setX(targetLeftOffset);
        quad[1].setX(targetRightOffset);
        quad[2].setX(targetRightOffset);

        const qreal leftShift = sideShift(params, leftScale, targetLeftOffset);
        const qreal rightShift = sideShift(params, rightScale, targetRightOffset);
        quad[0].setY(quad[0].y() + leftShift);
        quad[3].setY(quad[3].y() + leftShift);
        quad[1].setY(quad[1].y() + rightShift);
        quad[2].setY(quad[2].y() + rightShift);
    }
}

static void transformQuadsBottom(
    const KWin::EffectWindow* window,
    const TransformParameters& params,
    KWin::WindowQuadList& quads)
{
    const QRectF iconRect = params.iconRect;
    const QRectF windowRect = window->frameGeometry();

    const qreal distance = iconRect.top() - windowRect.top() + params.bumpDistance;

    for (int i = 0; i < quads.count(); ++i) {
        KWin::WindowQuad& quad = quads[i];

        const qreal topOffset = quad[0].y() + interpolate(0.0, distance, params.squashProgress);
        const qreal bottomOffset = quad[2].y() + interpolate(0.0, distance, params.squashProgress);

        const qreal topScale = params.stretchProgress * params.shapeCurve.valueForProgress(topOffset / distance);
        const qreal bottomScale = params.stretchProgress * params.shapeCurve.valueForProgress(bottomOffset / distance);

        const qreal targetTopLeftX = iconRect.x() + iconRect.width() * quad[0].x() / windowRect.width();
        const qreal targetTopRightX = iconRect.x() + iconRect.width() * quad[1].x() / windowRect.width();
        const qreal targetBottomRightX = iconRect.x() + iconRect.width() * quad[2].x() / windowRect.width();
        const qreal targetBottomLeftX = iconRect.x() + iconRect.width() * quad[3].x() / windowRect.width();

        quad[0].setX(quad[0].x() + topScale * (targetTopLeftX - (windowRect.x() + quad[0].x())));
        quad[1].setX(quad[1].x() + topScale * (targetTopRightX - (windowRect.x() + quad[1].x())));
        quad[2].setX(quad[2].x() + bottomScale * (targetBottomRightX - (windowRect.x() + quad[2].x())));
        quad[3].setX(quad[3].x() + bottomScale * (targetBottomLeftX - (windowRect.x() + quad[3].x())));

        const qreal targetTopOffset = topOffset - params.bumpDistance * params.bumpProgress;
        const qreal targetBottomOffset = bottomOffset - params.bumpDistance * params.bumpProgress;

        quad[0].setY(targetTopOffset);
        quad[1].setY(targetTopOffset);
        quad[2].setY(targetBottomOffset);
        quad[3].setY(targetBottomOffset);

        const qreal topShift = sideShift(params, topScale, targetTopOffset);
        const qreal bottomShift = sideShift(params, bottomScale, targetBottomOffset);
        quad[0].setX(quad[0].x() + topShift);
        quad[1].setX(quad[1].x() + topShift);
        quad[2].setX(quad[2].x() + bottomShift);
        quad[3].setX(quad[3].x() + bottomShift);
    }
}

static void transformQuads(
    const KWin::EffectWindow* window,
    const TransformParameters& params,
    KWin::WindowQuadList& quads)
{
    switch (params.direction) {
    case Direction::Left:
        transformQuadsLeft(window, params, quads);
        break;

    case Direction::Top:
        transformQuadsTop(window, params, quads);
        break;

    case Direction::Right:
        transformQuadsRight(window, params, quads);
        break;

    case Direction::Bottom:
        transformQuadsBottom(window, params, quads);
        break;

    default:
        Q_UNREACHABLE();
    }
}

void Model::apply(KWin::WindowQuadList& quads) const
{
    switch (m_stage) {
    case AnimationStage::Bump:
        applyBump(quads);
        break;

    case AnimationStage::Travel:
        applyTravel(quads);
        break;

    case AnimationStage::Settle:
        applySettle(quads);
        break;
    }
}

static TransformParameters baseParameters(const Model::Parameters& p, Direction direction,
    const QRectF& iconRect, int bumpDistance)
{
    TransformParameters params;
    params.shapeCurve = p.shapeCurve;
    params.direction = direction;
    params.iconRect = iconRect;
    params.bumpDistance = bumpDistance;
    params.squashProgress = 0.0;
    params.stretchProgress = 0.0;
    params.bumpProgress = 0.0;
    return params;
}

void Model::applyBump(KWin::WindowQuadList& quads) const
{
    TransformParameters params = baseParameters(m_parameters, m_direction, m_iconGeometry, m_bumpDistance);
    params.bumpProgress = m_timeLine.value();
    transformQuads(m_window, params, quads);
}

void Model::applyTravel(KWin::WindowQuadList& quads) const
{
    TransformParameters params = baseParameters(m_parameters, m_direction, m_iconGeometry, m_bumpDistance);
    const qreal g = travelProgress();
    if (g < m_split) {
        const qreal local = m_split > 0.0 ? g / m_split : 1.0;
        params.stretchProgress = m_shapeFactor * local;
        // unminimize: the bump comes back together with the stretch
        params.bumpProgress = m_coupledBump ? params.stretchProgress : 1.0;
    } else {
        const qreal local = m_split < 1.0 ? (g - m_split) / (1.0 - m_split) : 1.0;
        params.squashProgress = local;
        params.stretchProgress = qMin(m_shapeFactor + local, 1.0);
        params.bumpProgress = 1.0;
    }

    // Sideways extras are measured against the window size, so they look the
    // same on small and big windows.
    const QRectF windowRect = m_window->frameGeometry();
    const bool vertical = m_direction == Direction::Top || m_direction == Direction::Bottom;
    const qreal across = vertical ? windowRect.width() : windowRect.height();
    const qreal alongExtent = vertical ? windowRect.height() : windowRect.width();
    if (!qFuzzyIsNull(m_parameters.neckCurve)) {
        // bend towards the side the icon is on (or away with negative values)
        const qreal iconSide = vertical ? m_iconGeometry.center().x() - windowRect.center().x()
                                        : m_iconGeometry.center().y() - windowRect.center().y();
        params.bend = m_parameters.neckCurve * 0.35 * across * (iconSide < 0 ? -1.0 : 1.0);
    }
    if (m_parameters.wave > 0.0) {
        params.waveAmplitude = m_parameters.wave * 0.06 * across;
        params.waveLength = qMax(alongExtent * 0.45, 40.0);
        params.wavePhase = qMax(g, 0.0) * 3.0 * M_PI;
    }
    transformQuads(m_window, params, quads);
}

void Model::applySettle(KWin::WindowQuadList& quads) const
{
    // A decaying wobble around the window centre: big first swing, then calm.
    const qreal t = m_timeLine.value();
    const qreal amount = m_parameters.spring * 0.06 * std::sin(2.0 * M_PI * 1.75 * t) * std::exp(-3.5 * t) * (1.0 - t);
    if (qFuzzyIsNull(amount)) {
        return;
    }
    const QRectF windowRect = m_window->frameGeometry();
    const qreal cx = windowRect.width() / 2.0;
    const qreal cy = windowRect.height() / 2.0;
    // stretch along the motion axis, squeeze across it: like jelly
    const bool vertical = m_direction == Direction::Top || m_direction == Direction::Bottom;
    const qreal sx = vertical ? 1.0 - amount * 0.5 : 1.0 + amount;
    const qreal sy = vertical ? 1.0 + amount : 1.0 - amount * 0.5;
    for (int i = 0; i < quads.count(); ++i) {
        KWin::WindowQuad& quad = quads[i];
        for (int v = 0; v < 4; ++v) {
            quad[v].setX(cx + (quad[v].x() - cx) * sx);
            quad[v].setY(cy + (quad[v].y() - cy) * sy);
        }
    }
}

qreal Model::opacity() const
{
    if (m_stage != AnimationStage::Travel || m_parameters.fade <= 0.0) {
        return 1.0;
    }
    // fade only in the last part of the flight, smoothly
    const qreal g = qBound(0.0, travelProgress(), 1.0);
    const qreal e = qBound(0.0, (g - 0.45) / 0.55, 1.0);
    const qreal smooth = e * e * (3.0 - 2.0 * e);
    return 1.0 - m_parameters.fade * smooth;
}

Model::Parameters Model::parameters() const
{
    return m_parameters;
}

void Model::setParameters(const Parameters& parameters)
{
    m_parameters = parameters;
}

KWin::EffectWindow* Model::window() const
{
    return m_window;
}

void Model::setWindow(KWin::EffectWindow* window)
{
    m_window = window;
    m_iconGeometry = QRectF(); // reset so start() re-reads from window or override
}

void Model::setIconGeometry(const QRectF& rect)
{
    m_iconGeometry = rect;
}

bool Model::needsClip() const
{
    return m_clip;
}

QRegion Model::clipRegion() const
{
    const QRectF iconRect = m_iconGeometry;
    QRectF clipRect = m_window->expandedGeometry();

    switch (m_direction) {
    case Direction::Top:
        clipRect.translate(0, m_bumpDistance);
        clipRect.setTop(iconRect.top());
        clipRect.setLeft(qMin(iconRect.left(), clipRect.left()));
        clipRect.setRight(qMax(iconRect.right(), clipRect.right()));
        break;

    case Direction::Right:
        clipRect.translate(-m_bumpDistance, 0);
        clipRect.setRight(iconRect.right());
        clipRect.setTop(qMin(iconRect.top(), clipRect.top()));
        clipRect.setBottom(qMax(iconRect.bottom(), clipRect.bottom()));
        break;

    case Direction::Bottom:
        clipRect.translate(0, -m_bumpDistance);
        clipRect.setBottom(iconRect.bottom());
        clipRect.setLeft(qMin(iconRect.left(), clipRect.left()));
        clipRect.setRight(qMax(iconRect.right(), clipRect.right()));
        break;

    case Direction::Left:
        clipRect.translate(m_bumpDistance, 0);
        clipRect.setLeft(iconRect.left());
        clipRect.setTop(qMin(iconRect.top(), clipRect.top()));
        clipRect.setBottom(qMax(iconRect.bottom(), clipRect.bottom()));
        break;

    default:
        Q_UNREACHABLE();
    }

    return clipRect.toAlignedRect();
}

int Model::computeBumpDistance() const
{
    const QRectF windowRect = m_window->frameGeometry();
    const QRectF iconRect = m_iconGeometry;

    qreal bumpDistance = 0;
    switch (m_direction) {
    case Direction::Top:
        bumpDistance = std::max(qreal(0), iconRect.y() + iconRect.height() - windowRect.y());
        break;

    case Direction::Right:
        bumpDistance = std::max(qreal(0), windowRect.x() + windowRect.width() - iconRect.x());
        break;

    case Direction::Bottom:
        bumpDistance = std::max(qreal(0), windowRect.y() + windowRect.height() - iconRect.y());
        break;

    case Direction::Left:
        bumpDistance = std::max(qreal(0), iconRect.x() + iconRect.width() - windowRect.x());
        break;

    default:
        Q_UNREACHABLE();
    }

    bumpDistance += std::min(bumpDistance, m_parameters.bumpDistance);

    return bumpDistance;
}

qreal Model::computeShapeFactor() const
{
    const QRectF windowRect = m_window->frameGeometry();
    const QRectF iconRect = m_iconGeometry;

    int movingExtent = 0;
    int distanceToIcon = 0;
    switch (m_direction) {
    case Direction::Top:
        movingExtent = windowRect.height();
        distanceToIcon = windowRect.bottom() - iconRect.bottom() + m_bumpDistance;
        break;

    case Direction::Right:
        movingExtent = windowRect.width();
        distanceToIcon = iconRect.left() - windowRect.left() + m_bumpDistance;
        break;

    case Direction::Bottom:
        movingExtent = windowRect.height();
        distanceToIcon = iconRect.top() - windowRect.top() + m_bumpDistance;
        break;

    case Direction::Left:
        movingExtent = windowRect.width();
        distanceToIcon = windowRect.right() - iconRect.right() + m_bumpDistance;
        break;

    default:
        Q_UNREACHABLE();
    }

    const qreal minimumShapeFactor = static_cast<qreal>(movingExtent) / distanceToIcon;
    return qMax(m_parameters.shapeFactor, minimumShapeFactor);
}
