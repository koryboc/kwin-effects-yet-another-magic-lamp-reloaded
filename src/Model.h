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

#pragma once

// Own
#include "common.h"

// kwineffects
#include <effect/effecthandler.h>
#include <effect/effectwindow.h>
#include <effect/timeline.h>
#include <scene/itemgeometry.h>

/**
 * Model for the magic lamp animation.
 **/
class Model {
public:
    struct Parameters {
        // The duration of the squash stage.
        std::chrono::milliseconds squashDuration;

        // The duration of the stretch stage.
        std::chrono::milliseconds stretchDuration;

        // The duration of the Stretch2 stage on unminimize (contains the
        // coupled bump-return). Defaults to stretchDuration.
        std::chrono::milliseconds unminimizeStretchDuration;

        // How long it takes to raise the window.
        std::chrono::milliseconds bumpDuration;

        // Defines shape of transformed windows.
        QEasingCurve shapeCurve;

        // The blend factor between Squash and Stretch stage.
        qreal shapeFactor;

        // How much the transformed window should be raised.
        qreal bumpDistance;

        // Speed profile of the whole flight into/out of the icon. The
        // stretch and squash parts share one curve, so the speed never jumps
        // between them.
        QEasingCurve motionCurve = QEasingCurve(QEasingCurve::Linear);

        // 0..1: springy overshoot when a window comes back (unminimize).
        qreal spring = 0.0;
        std::chrono::milliseconds settleDuration = std::chrono::milliseconds(450);

        // 0..1: how much the window fades out as it reaches the icon.
        qreal fade = 0.0;

        // 0..1: cloth-like ripple along the neck.
        qreal wave = 0.0;

        // -1..1: the neck bends into an arc instead of a straight funnel.
        qreal neckCurve = 0.0;
    };

    explicit Model(KWin::EffectWindow* window = nullptr);

    /**
     * This enum type is used to specify kind of the current animation.
     **/
    enum class AnimationKind {
        Minimize,
        Unminimize
    };

    /**
     * Starts animation with the given kind.
     *
     * @param kind Kind of the animation (minimize or unminimize).
     **/
    void start(AnimationKind kind);

    /**
     * Updates the model to @p milliseconds.
     **/
    void advance(std::chrono::milliseconds presentTime);

    /**
     * Returns whether the animation is complete.
     **/
    bool done() const;

    /**
     * Applies the current state of the model to the given list of
     * window quads.
     *
     * @param quads The list of window quads to be transformed.
     **/
    void apply(KWin::WindowQuadList& quads) const;

    /**
     * Returns the parameters of the model.
     **/
    Parameters parameters() const;

    /**
     * Sets parameters of the model.
     *
     * @param parameters The new paramaters.
     **/
    void setParameters(const Parameters& parameters);

    /**
     * Returns the associated window.
     **/
    KWin::EffectWindow* window() const;

    /**
     * Sets the associated window.
     *
     * @param window The window associated with this model.
     **/
    void setWindow(KWin::EffectWindow* window);

    /**
     * Overrides the icon geometry used for the animation target.
     * Call before start() to use a different target than iconGeometry().
     **/
    void setIconGeometry(const QRectF& rect);

    /**
     * Returns whether the painted result has to be clipped.
     *
     * @see clipRegion
     **/
    bool needsClip() const;

    /**
     * Returns the clip region.
     *
     * @see needsClip
     **/
    QRegion clipRegion() const;

    /**
     * Opacity factor for the window at the current moment (fade option).
     **/
    qreal opacity() const;

private:
    void applyBump(KWin::WindowQuadList& quads) const;
    void applyTravel(KWin::WindowQuadList& quads) const;
    void applySettle(KWin::WindowQuadList& quads) const;

    void startBump(KWin::TimeLine::Direction direction);
    void startTravel(KWin::TimeLine::Direction direction, bool coupledBump);
    void startSettleOrFinish();
    void updateClip();
    qreal travelProgress() const; // eased 0..1 (may dip below 0 with "anticipate")

    int computeBumpDistance() const;
    qreal computeShapeFactor() const;

    Parameters m_parameters;

    // Bump: the window is raised above the panel first.
    // Travel: stretch + squash into the icon as one continuous flight.
    // Settle: springy wobble after the window is back (unminimize only).
    enum class AnimationStage {
        Bump,
        Travel,
        Settle
    };

    KWin::EffectWindow* m_window;
    QRectF m_iconGeometry;
    AnimationKind m_kind;
    AnimationStage m_stage;
    KWin::TimeLine m_timeLine;
    Direction m_direction;
    int m_bumpDistance;
    qreal m_shapeFactor;
    qreal m_split = 0.5;          // part of the flight spent stretching
    bool m_coupledBump = false;   // unminimize: the bump returns while stretching
    bool m_clip;
    bool m_done = false;
};
