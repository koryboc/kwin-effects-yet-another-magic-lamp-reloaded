/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 * Copyright (C) 2026 Roy Bock <koryboc@hotmail.com>
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
#include "YetAnotherMagicLampEffectKCM.h"
#include "Presets.h"

// Auto-generated
#include "YetAnotherMagicLampConfig.h"
#include "kwineffects_interface.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QSlider>
#include <QSpinBox>

#include <cmath>

static const char* const kDurationNames[] = { "Duration", "StretchDuration", "UnminimizeStretchDuration",
    "SquashDuration", "BumpDuration" };

YetAnotherMagicLampEffectKCM::YetAnotherMagicLampEffectKCM(QObject* parent, const KPluginMetaData& data)
    : KCModule(parent, data)
{
    auto* form = new QFormLayout(widget());
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // ---- preset and language ----------------------------------------------
    m_preset = new QComboBox(widget());
    m_preset->setObjectName(QStringLiteral("kcfg_Preset"));
    m_preset->addItem(QString()); // Custom
    for (int i = 0; i < kPresetCount; ++i) {
        m_preset->addItem(QString());
    }
    addRow("preset", m_preset);
    m_presetDescription = new QLabel(widget());
    m_presetDescription->setWordWrap(true);
    m_presetDescription->setEnabled(false); // greyed, like a hint
    form->addRow(QString(), m_presetDescription);

    // Manually bound (QComboBox has no Q_PROPERTY for currentData, so
    // kcfg auto-binding cannot store the language code as a string).
    m_language = new QComboBox(widget());
    m_language->addItem(QString(), QString());
    m_language->addItem(QStringLiteral("English"), QStringLiteral("en"));
    m_language->addItem(QStringLiteral("Deutsch"), QStringLiteral("de"));
    m_language->addItem(QStringLiteral("Русский"), QStringLiteral("ru"));
    m_language->addItem(QStringLiteral("Українська"), QStringLiteral("uk"));
    addRow("language", m_language);

    // ---- shape ---------------------------------------------------------------
    addSection("sec_shape");
    m_shapeCurve = new QComboBox(widget());
    m_shapeCurve->setObjectName(QStringLiteral("kcfg_ShapeCurve"));
    for (int i = 0; i < 9; ++i) {
        m_shapeCurve->addItem(QString());
    }
    addRow("ShapeCurve", m_shapeCurve);
    m_shapeCurveDescription = new QLabel(widget());
    m_shapeCurveDescription->setWordWrap(true);
    m_shapeCurveDescription->setEnabled(false);
    form->addRow(QString(), m_shapeCurveDescription);

    m_shapeFactor = new QDoubleSpinBox(widget());
    m_shapeFactor->setObjectName(QStringLiteral("kcfg_InitialShapeFactor"));
    m_shapeFactor->setRange(0.0, 1.0);
    m_shapeFactor->setSingleStep(0.05);
    addRow("InitialShapeFactor", m_shapeFactor);

    addPercent("NeckCurve", -100);
    addPercent("Wave", 0);
    addPercent("Fade", 0);

    // ---- motion --------------------------------------------------------------
    addSection("sec_motion");
    m_motion = new QComboBox(widget());
    m_motion->setObjectName(QStringLiteral("kcfg_Motion"));
    for (int i = 0; i < 5; ++i) {
        m_motion->addItem(QString());
    }
    addRow("Motion", m_motion);
    m_motionDescription = new QLabel(widget());
    m_motionDescription->setWordWrap(true);
    m_motionDescription->setEnabled(false);
    form->addRow(QString(), m_motionDescription);
    addPercent("Spring", 0);

    // ---- timing --------------------------------------------------------------
    addSection("sec_timing");
    for (const char* name : kDurationNames) {
        addDuration(name);
    }

    // ---- advanced ------------------------------------------------------------
    addSection("sec_advanced");
    m_maxBump = new QSpinBox(widget());
    m_maxBump->setObjectName(QStringLiteral("kcfg_MaxBumpDistance"));
    m_maxBump->setRange(0, 500);
    addRow("MaxBumpDistance", m_maxBump);
    m_grid = new QSpinBox(widget());
    m_grid->setObjectName(QStringLiteral("kcfg_GridResolution"));
    m_grid->setRange(4, 120);
    addRow("GridResolution", m_grid);

    m_hint = new QLabel(widget());
    m_hint->setWordWrap(true);
    m_hint->setEnabled(false);
    form->addRow(m_hint);

    addConfig(YetAnotherMagicLampConfig::self(), widget());

    // Choosing a preset fills in every field; touching any field afterwards
    // shows "Custom" (or the preset it happens to match again).
    connect(m_preset, &QComboBox::activated, this, &YetAnotherMagicLampEffectKCM::applyPreset);
    connect(m_preset, &QComboBox::currentIndexChanged, this, &YetAnotherMagicLampEffectKCM::retranslate);
    connect(m_language, &QComboBox::currentIndexChanged, this, &YetAnotherMagicLampEffectKCM::retranslate);
    connect(m_language, &QComboBox::currentIndexChanged, this, [this]() {
        setNeedsSave(true);
    });
    connect(m_shapeCurve, &QComboBox::currentIndexChanged, this, &YetAnotherMagicLampEffectKCM::retranslate);
    connect(m_motion, &QComboBox::currentIndexChanged, this, &YetAnotherMagicLampEffectKCM::retranslate);
    const auto sync = [this]() {
        syncPresetFromValues();
    };
    connect(m_shapeCurve, &QComboBox::currentIndexChanged, this, sync);
    connect(m_motion, &QComboBox::currentIndexChanged, this, sync);
    connect(m_shapeFactor, &QDoubleSpinBox::valueChanged, this, sync);
    connect(m_maxBump, &QSpinBox::valueChanged, this, sync);
    connect(m_grid, &QSpinBox::valueChanged, this, sync);
    for (QSpinBox* box : std::as_const(m_durations)) {
        connect(box, &QSpinBox::valueChanged, this, sync);
    }
    for (QSlider* slider : std::as_const(m_percents)) {
        connect(slider, &QSlider::valueChanged, this, sync);
    }

    retranslate();
}

QSpinBox* YetAnotherMagicLampEffectKCM::addDuration(const char* name)
{
    auto* box = new QSpinBox(widget());
    box->setObjectName(QStringLiteral("kcfg_") + QLatin1String(name));
    box->setRange(0, 9999); // 0 = KWin's default speed
    box->setSingleStep(10);
    m_durations.insert(QLatin1String(name), box);
    addRow(name, box);
    return box;
}

QSlider* YetAnotherMagicLampEffectKCM::addPercent(const char* name, int minimum)
{
    auto* row = new QWidget(widget());
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    auto* slider = new QSlider(Qt::Horizontal, row);
    slider->setObjectName(QStringLiteral("kcfg_") + QLatin1String(name));
    slider->setRange(minimum, 100);
    slider->setPageStep(10);
    auto* value = new QLabel(row);
    value->setMinimumWidth(value->fontMetrics().horizontalAdvance(QStringLiteral("-100 %  ")));
    value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(slider, 1);
    layout->addWidget(value);
    m_percents.insert(QLatin1String(name), slider);
    m_percentValues.insert(QLatin1String(name), value);
    connect(slider, &QSlider::valueChanged, this, &YetAnotherMagicLampEffectKCM::retranslate);
    addRow(name, row);
    return slider;
}

void YetAnotherMagicLampEffectKCM::addRow(const char* key, QWidget* field)
{
    auto* label = new QLabel(widget());
    label->setBuddy(field);
    m_labels.append({ label, QByteArray(key) });
    static_cast<QFormLayout*>(widget()->layout())->addRow(label, field);
}

void YetAnotherMagicLampEffectKCM::addSection(const char* key)
{
    auto* label = new QLabel(widget());
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);
    label->setContentsMargins(0, label->fontMetrics().height() / 2, 0, 0);
    m_labels.append({ label, QByteArray(key) });
    static_cast<QFormLayout*>(widget()->layout())->addRow(label);
}

QByteArray YetAnotherMagicLampEffectKCM::language() const
{
    const QByteArray code = m_language ? m_language->currentData().toString().toLatin1() : QByteArray();
    if (!code.isEmpty()) {
        return code;
    }
    // "System default": follow the language KDE uses
    const QString system = QLocale().name();
    if (system.startsWith(QLatin1String("de")))
        return "de";
    if (system.startsWith(QLatin1String("ru")))
        return "ru";
    if (system.startsWith(QLatin1String("uk")))
        return "uk";
    return "en";
}

void YetAnotherMagicLampEffectKCM::retranslate()
{
    const QByteArray lang = language();
    for (const auto& entry : std::as_const(m_labels)) {
        entry.first->setText(lampText(entry.second.constData(), lang));
        const QString tip = lampTip(entry.second, lang);
        entry.first->setToolTip(tip);
        if (QWidget* field = entry.first->buddy()) {
            field->setToolTip(tip);
        }
    }
    m_language->setItemText(0, lampText("system", lang));
    m_preset->setItemText(0, lampText("custom", lang));
    for (int i = 0; i < kPresetCount; ++i) {
        m_preset->setItemText(i + 1, lampText(kPresets[i].key, lang));
    }
    const int preset = m_preset->currentIndex();
    const QByteArray descKey = (preset >= 1 && preset <= kPresetCount ? QByteArray(kPresets[preset - 1].key)
                                                                      : QByteArray("custom"))
        + "_desc";
    m_presetDescription->setText(lampText(descKey.constData(), lang));
    for (int i = 0; i < m_shapeCurve->count(); ++i) {
        const QByteArray key = "curve" + QByteArray::number(i);
        m_shapeCurve->setItemText(i, lampText(key.constData(), lang));
        m_shapeCurve->setItemData(i, lampTip(key, lang), Qt::ToolTipRole);
    }
    for (int i = 0; i < m_motion->count(); ++i) {
        const QByteArray key = "motion" + QByteArray::number(i);
        m_motion->setItemText(i, lampText(key.constData(), lang));
        m_motion->setItemData(i, lampTip(key, lang), Qt::ToolTipRole);
    }
    m_shapeCurveDescription->setText(lampTip("curve" + QByteArray::number(m_shapeCurve->currentIndex()), lang));
    m_motionDescription->setText(lampTip("motion" + QByteArray::number(m_motion->currentIndex()), lang));
    for (QSpinBox* box : std::as_const(m_durations)) {
        box->setSpecialValueText(lampText("default", lang));
        box->setSuffix(lampText("ms", lang));
    }
    m_maxBump->setSuffix(lampText("px", lang));
    for (auto it = m_percents.constBegin(); it != m_percents.constEnd(); ++it) {
        const int v = it.value()->value();
        m_percentValues.value(it.key())->setText(v == 0 ? lampText("off", lang) : QStringLiteral("%1 %").arg(v));
    }
    m_hint->setText(lampText("hint", lang));
}

void YetAnotherMagicLampEffectKCM::applyPreset(int index)
{
    if (index < 1 || index > kPresetCount) {
        return; // "Custom": keep whatever is set
    }
    const LampPreset& p = kPresets[index - 1];
    m_applyingPreset = true;
    m_durations.value(QStringLiteral("Duration"))->setValue(0);
    m_durations.value(QStringLiteral("StretchDuration"))->setValue(p.stretch);
    m_durations.value(QStringLiteral("UnminimizeStretchDuration"))->setValue(p.unminimizeStretch);
    m_durations.value(QStringLiteral("SquashDuration"))->setValue(p.squash);
    m_durations.value(QStringLiteral("BumpDuration"))->setValue(p.bump);
    m_maxBump->setValue(p.maxBumpDistance);
    m_grid->setValue(p.gridResolution);
    m_shapeFactor->setValue(p.shapeFactor);
    m_shapeCurve->setCurrentIndex(p.shapeCurve);
    m_motion->setCurrentIndex(p.motion);
    m_percents.value(QStringLiteral("Spring"))->setValue(p.spring);
    m_percents.value(QStringLiteral("Fade"))->setValue(p.fade);
    m_percents.value(QStringLiteral("Wave"))->setValue(p.wave);
    m_percents.value(QStringLiteral("NeckCurve"))->setValue(p.neckCurve);
    m_applyingPreset = false;
    syncPresetFromValues();
}

void YetAnotherMagicLampEffectKCM::syncPresetFromValues()
{
    if (m_applyingPreset) {
        return;
    }
    const auto duration = [this](const char* name) {
        return m_durations.value(QLatin1String(name))->value();
    };
    const auto percent = [this](const char* name) {
        return m_percents.value(QLatin1String(name))->value();
    };
    int match = 0;
    for (int i = 0; i < kPresetCount && match == 0; ++i) {
        const LampPreset& p = kPresets[i];
        if (duration("Duration") == 0 && duration("StretchDuration") == p.stretch
            && duration("UnminimizeStretchDuration") == p.unminimizeStretch
            && duration("SquashDuration") == p.squash && duration("BumpDuration") == p.bump
            && m_maxBump->value() == p.maxBumpDistance && m_grid->value() == p.gridResolution
            && std::abs(m_shapeFactor->value() - p.shapeFactor) < 0.001
            && m_shapeCurve->currentIndex() == p.shapeCurve && m_motion->currentIndex() == p.motion
            && percent("Spring") == p.spring && percent("Fade") == p.fade && percent("Wave") == p.wave
            && percent("NeckCurve") == p.neckCurve) {
            match = i + 1;
        }
    }
    if (m_preset->currentIndex() != match) {
        m_preset->setCurrentIndex(match); // KConfig sees the change and enables Apply
    }
}

void YetAnotherMagicLampEffectKCM::load()
{
    KCModule::load();
    const QString code = YetAnotherMagicLampConfig::language();
    int idx = m_language->findData(code);
    if (idx < 0) {
        idx = 0;
    }
    m_language->setCurrentIndex(idx);
    syncPresetFromValues();
    retranslate();
}

void YetAnotherMagicLampEffectKCM::defaults()
{
    KCModule::defaults();
    m_language->setCurrentIndex(0);
    syncPresetFromValues();
    retranslate();
}

void YetAnotherMagicLampEffectKCM::save()
{
    YetAnotherMagicLampConfig::setLanguage(m_language->currentData().toString());
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
        QStringLiteral("/Effects"),
        QDBusConnection::sessionBus());
    // KF6 takes the plugin id from the file name (lib…), not from metadata "Id"
    interface.reconfigureEffect(QStringLiteral("libkwin4_effect_yetanothermagiclamp"));
}
