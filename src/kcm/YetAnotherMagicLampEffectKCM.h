/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
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

// KF6
#include <KCModule>
#include <KPluginMetaData>

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QPair>

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QSlider;
class QSpinBox;

// The settings page is built in code (not a .ui file) so every text can be
// switched between English, Ukrainian and Russian on the fly.
class YetAnotherMagicLampEffectKCM : public KCModule {
    Q_OBJECT

public:
    explicit YetAnotherMagicLampEffectKCM(QObject* parent, const KPluginMetaData& data);

    void load() override;
    void save() override;
    void defaults() override;

private:
    QSpinBox* addDuration(const char* name);
    QSlider* addPercent(const char* name, int minimum);
    void addRow(const char* key, QWidget* field);
    void addSection(const char* key);

    int language() const; // 0 English, 1 Ukrainian, 2 Russian
    void retranslate();
    void applyPreset(int index);
    void syncPresetFromValues();

    QComboBox* m_preset = nullptr;
    QComboBox* m_language = nullptr;
    QLabel* m_presetDescription = nullptr;
    QLabel* m_hint = nullptr;
    QComboBox* m_shapeCurve = nullptr;
    QLabel* m_shapeCurveDescription = nullptr;
    QDoubleSpinBox* m_shapeFactor = nullptr;
    QComboBox* m_motion = nullptr;
    QLabel* m_motionDescription = nullptr;
    QSpinBox* m_maxBump = nullptr;
    QSpinBox* m_grid = nullptr;
    QHash<QString, QSpinBox*> m_durations;
    QHash<QString, QSlider*> m_percents;
    QHash<QString, QLabel*> m_percentValues;
    QList<QPair<QLabel*, QByteArray>> m_labels; // label, translation key
    bool m_applyingPreset = false;
};
