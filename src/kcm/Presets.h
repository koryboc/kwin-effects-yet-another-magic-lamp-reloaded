/*
 * Presets and translations for the settings page.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <QString>

// One complete look. Durations of 0 mean "KWin's default speed".
struct LampPreset {
    const char* key; // translation key of the name; description key is key + "_desc"
    int stretch, unminimizeStretch, squash, bump;
    int maxBumpDistance, gridResolution;
    double shapeFactor;
    int shapeCurve; // 0 Linear .. 8 Bezier
    int motion;     // 0 even, 1 accelerate, 2 smooth, 3 decelerate, 4 anticipate
    int spring, fade, wave, neckCurve; // percent
};

// Index 0 in the list is "Custom"; presets start at 1 (same as the config value).
inline constexpr LampPreset kPresets[] = {
    {"classic", 0, 0, 0, 0, 0, 30, 0.20, 2, 0, 0, 0, 0, 0},
    {"genie", 200, 200, 260, 0, 0, 40, 0.35, 5, 1, 0, 0, 0, 0},
    {"soft", 300, 300, 380, 220, 20, 40, 0.25, 5, 2, 0, 25, 0, 0},
    {"fast", 110, 110, 150, 0, 0, 24, 0.30, 2, 1, 0, 0, 0, 0},
    {"springy", 220, 200, 260, 180, 30, 36, 0.25, 2, 2, 60, 0, 0, 0},
    {"jelly", 260, 240, 300, 0, 0, 48, 0.25, 4, 4, 100, 0, 30, 0},
    {"ghost", 260, 260, 320, 0, 0, 30, 0.25, 5, 3, 0, 100, 0, 0},
    {"silk", 300, 300, 360, 0, 0, 60, 0.30, 5, 2, 0, 0, 55, 0},
    {"whirl", 220, 220, 280, 0, 0, 48, 0.30, 1, 1, 0, 0, 0, 70},
    {"smoke", 280, 260, 340, 0, 0, 48, 0.30, 5, 3, 0, 70, 25, -30},
};
inline constexpr int kPresetCount = int(sizeof(kPresets) / sizeof(kPresets[0]));

// 0 English, 1 Ukrainian, 2 Russian
struct LampText {
    const char* key;
    const char* text[3];
};

// clang-format off
inline constexpr LampText kTexts[] = {
    {"preset",        {"Preset:", "Пресет:", "Пресет:"}},
    {"language",      {"Language:", "Мова:", "Язык:"}},
    {"system",        {"System default", "Як у системі", "Как в системе"}},
    {"custom",        {"Custom", "Власний", "Свой"}},
    {"custom_desc",   {"Your own combination of the settings below.",
                       "Ваше власне поєднання налаштувань нижче.",
                       "Ваше собственное сочетание настроек ниже."}},
    {"classic",       {"Classic", "Класичний", "Классический"}},
    {"classic_desc",  {"The original Yet Another Magic Lamp.",
                       "Оригінальний Yet Another Magic Lamp.",
                       "Оригинальный Yet Another Magic Lamp."}},
    {"genie",         {"Genie (macOS-like)", "Джин (як у macOS)", "Джинн (как в macOS)"}},
    {"genie_desc",    {"Narrow neck, the window speeds up as it is sucked into the panel.",
                       "Вузька шия, вікно розганяється, коли його затягує в панель.",
                       "Узкая шея, окно разгоняется, когда его затягивает в панель."}},
    {"soft",          {"Soft", "М'який", "Мягкий"}},
    {"soft_desc",     {"Slow and smooth, with a gentle lift and a little fading.",
                       "Повільно й плавно, з легким підйомом і трохи згасає.",
                       "Медленно и плавно, с лёгким подъёмом и немного гаснет."}},
    {"fast",          {"Fast", "Швидкий", "Быстрый"}},
    {"fast_desc",     {"Quick and snappy, for everyday work.",
                       "Швидко й чітко, для щоденної роботи.",
                       "Быстро и чётко, для ежедневной работы."}},
    {"springy",       {"Springy", "Пружний", "Пружинящий"}},
    {"springy_desc",  {"Windows bounce a little when they come back.",
                       "Вікна трохи пружинять, коли повертаються.",
                       "Окна немного пружинят, когда возвращаются."}},
    {"jelly",         {"Jelly", "Желе", "Желе"}},
    {"jelly_desc",    {"A pull back before the jump, a wobbly neck and a strong wobble on return.",
                       "Замах перед стрибком, хвиляста шия і сильне тремтіння при поверненні.",
                       "Замах перед прыжком, волнистая шея и сильное дрожание при возвращении."}},
    {"ghost",         {"Ghost", "Привид", "Призрак"}},
    {"ghost_desc",    {"The window melts away as it reaches the panel.",
                       "Вікно тане, коли долітає до панелі.",
                       "Окно тает, когда долетает до панели."}},
    {"silk",          {"Silk", "Шовк", "Шёлк"}},
    {"silk_desc",     {"The neck ripples like fabric in the wind.",
                       "Шия хвилюється, як тканина на вітрі.",
                       "Шея колышется, как ткань на ветру."}},
    {"whirl",         {"Whirlwind", "Вихор", "Вихрь"}},
    {"whirl_desc",    {"The neck curves in an arc towards the icon.",
                       "Шия вигинається дугою в бік іконки.",
                       "Шея изгибается дугой в сторону иконки."}},
    {"smoke",         {"Smoke", "Димок", "Дымка"}},
    {"smoke_desc",    {"A curved, rippling neck that fades like smoke.",
                       "Вигнута хвиляста шия, що розвіюється, як дим.",
                       "Изогнутая волнистая шея, которая рассеивается, как дым."}},

    {"sec_shape",     {"Shape", "Форма", "Форма"}},
    {"sec_motion",    {"Motion", "Рух", "Движение"}},
    {"sec_timing",    {"Timing", "Тривалість", "Длительность"}},
    {"sec_advanced",  {"Advanced", "Додатково", "Дополнительно"}},

    {"ShapeCurve",          {"Neck shape:", "Форма шиї:", "Форма шеи:"}},
    {"InitialShapeFactor",  {"Initial neck width:", "Початкова ширина шиї:", "Начальная ширина шеи:"}},
    {"NeckCurve",           {"Neck bend:", "Вигин шиї:", "Изгиб шеи:"}},
    {"Wave",                {"Ripple:", "Хвиля:", "Волна:"}},
    {"Fade",                {"Fade out:", "Згасання:", "Затухание:"}},
    {"Motion",              {"Speed profile:", "Характер руху:", "Характер движения:"}},
    {"Spring",              {"Spring on restore:", "Пружність при розгортанні:", "Пружинистость при разворачивании:"}},
    {"Duration",            {"Overall duration:", "Загальна тривалість:", "Общая длительность:"}},
    {"StretchDuration",     {"Stretch:", "Розтягування:", "Растяжение:"}},
    {"UnminimizeStretchDuration", {"Stretch on restore:", "Розтягування при розгортанні:", "Растяжение при разворачивании:"}},
    {"SquashDuration",      {"Squash:", "Стискання:", "Сжатие:"}},
    {"BumpDuration",        {"Lift:", "Підйом:", "Подъём:"}},
    {"MaxBumpDistance",     {"Lift height:", "Висота підйому:", "Высота подъёма:"}},
    {"GridResolution",      {"Mesh detail:", "Деталізація сітки:", "Детализация сетки:"}},

    {"curve0", {"Linear", "Лінійна", "Линейная"}},
    {"curve1", {"Quadratic", "Квадратична", "Квадратичная"}},
    {"curve2", {"Cubic", "Кубічна", "Кубическая"}},
    {"curve3", {"Quartic", "Четвертого степеня", "Четвёртой степени"}},
    {"curve4", {"Quintic", "П'ятого степеня", "Пятой степени"}},
    {"curve5", {"Sine", "Синусоїда", "Синусоида"}},
    {"curve6", {"Circular", "Колова", "Круговая"}},
    {"curve7", {"Bounce", "Стрибки", "Прыжки"}},
    {"curve8", {"Bezier", "Безьє", "Безье"}},

    {"motion0", {"Even (classic)", "Рівномірно (класично)", "Равномерно (классически)"}},
    {"motion1", {"Accelerate into the panel", "З розгоном у панель", "С разгоном в панель"}},
    {"motion2", {"Smooth start and end", "Плавно на початку й наприкінці", "Плавно в начале и в конце"}},
    {"motion3", {"Fast start, gentle arrival", "Швидкий старт, м'яке прибуття", "Быстрый старт, мягкое прибытие"}},
    {"motion4", {"Anticipate (pull back first)", "Із замахом", "С замахом"}},

    {"default", {"Default", "Типово", "По умолчанию"}},
    {"off",     {"Off", "Вимк.", "Выкл."}},
    {"ms",      {" ms", " мс", " мс"}},
    {"px",      {" px", " пкс", " пкс"}},
    {"hint",    {"Press Apply, then minimize any window to see it.",
                 "Натисніть «Застосувати» і згорніть будь-яке вікно, щоб побачити.",
                 "Нажмите «Применить» и сверните любое окно, чтобы увидеть."}},

    // Descriptions: "<key>_tip" is the tooltip of a row (label and field) or
    // of a combo box item; the selected curve/motion is also shown below it.
    {"ShapeCurve_tip",         {"Outline of the window's neck while it is pulled into the panel.",
                                "Обрис «шиї» вікна, поки його втягує в панель.",
                                "Очертание «шеи» окна, пока его втягивает в панель."}},
    {"InitialShapeFactor_tip", {"How narrowed the window already is when the animation starts: 0 starts from the full window, higher values start with a funnel.",
                                "Наскільки вікно вже звужене на початку анімації: 0 — починає з повного вікна, більші значення — одразу з «лійкою».",
                                "Насколько окно уже сужено в начале анимации: 0 — начинает с полного окна, большие значения — сразу с «воронкой»."}},
    {"NeckCurve_tip",          {"Bends the neck into an arc instead of a straight funnel; negative values bend it the other way. 0 = straight.",
                                "Вигинає шию дугою замість прямої «лійки»; від'ємні значення — в інший бік. 0 — пряма.",
                                "Изгибает шею дугой вместо прямой «воронки»; отрицательные значения — в другую сторону. 0 — прямая."}},
    {"Wave_tip",               {"Cloth-like ripple running along the neck. 0 = off.",
                                "Хвиля вздовж шиї, як тканина на вітрі. 0 — вимкнено.",
                                "Волна вдоль шеи, как ткань на ветру. 0 — выключено."}},
    {"Fade_tip",               {"How much the window fades out as it reaches the panel. 0 = stays opaque, 100 % = disappears completely.",
                                "Наскільки вікно зникає, підлітаючи до панелі. 0 — лишається непрозорим, 100 % — зникає повністю.",
                                "Насколько окно исчезает, подлетая к панели. 0 — остаётся непрозрачным, 100 % — исчезает полностью."}},
    {"Motion_tip",             {"How the speed changes during the flight. On restore it plays in reverse.",
                                "Як змінюється швидкість під час польоту. При розгортанні — у зворотному порядку.",
                                "Как меняется скорость во время полёта. При разворачивании — в обратном порядке."}},
    {"Spring_tip",             {"The window wobbles a little when it comes back from the panel. 0 = off.",
                                "Вікно трохи пружинить, коли повертається з панелі. 0 — вимкнено.",
                                "Окно немного пружинит, когда возвращается из панели. 0 — выключено."}},
    {"Duration_tip",           {"Length of the whole animation. \"Default\" uses KWin's animation speed and the phases below.",
                                "Тривалість усієї анімації. «Типово» — за швидкістю анімацій KWin і фазами нижче.",
                                "Длительность всей анимации. «По умолчанию» — по скорости анимаций KWin и фазам ниже."}},
    {"StretchDuration_tip",    {"Phase in which the window stretches into the neck when minimizing.",
                                "Фаза, коли вікно витягується в «шию» під час згортання.",
                                "Фаза, когда окно вытягивается в «шею» при сворачивании."}},
    {"UnminimizeStretchDuration_tip", {"The same stretch phase when the window is restored.",
                                "Та сама фаза розтягування, коли вікно розгортається.",
                                "Та же фаза растяжения, когда окно разворачивается."}},
    {"SquashDuration_tip",     {"Phase in which the stretched window is squeezed into the icon.",
                                "Фаза, коли витягнуте вікно стискається в значок.",
                                "Фаза, когда вытянутое окно сжимается в значок."}},
    {"BumpDuration_tip",       {"Phase in which the window lifts off the panel edge before flying.",
                                "Фаза, коли вікно піднімається над краєм панелі перед польотом.",
                                "Фаза, когда окно поднимается над краем панели перед полётом."}},
    {"MaxBumpDistance_tip",    {"How high the window lifts above the panel at most.",
                                "Наскільки найвище вікно піднімається над панеллю.",
                                "Насколько максимально окно поднимается над панелью."}},
    {"GridResolution_tip",     {"How finely the window is split for bending. Higher = smoother curves, slightly more GPU work.",
                                "Наскільки дрібно вікно ділиться для вигину. Більше — плавніші криві, трохи більше роботи для GPU.",
                                "Насколько мелко окно делится для изгиба. Больше — более плавные кривые, чуть больше работы для GPU."}},

    {"curve0_tip", {"Straight wedge, like a trapezoid.", "Рівний клин, як трапеція.", "Ровный клин, как трапеция."}},
    {"curve1_tip", {"Soft bend.", "М'який вигин.", "Мягкий изгиб."}},
    {"curve2_tip", {"Noticeable neck: wide on top, narrows sharply near the icon.",
                    "Помітна «шия»: угорі широке, біля значка різко звужується.",
                    "Заметная «шея»: вверху широкое, у значка резко сужается."}},
    {"curve3_tip", {"Even sharper: wide body, thin tail.", "Ще різкіше: широке «тіло», тонкий «хвіст».", "Ещё резче: широкое «тело», тонкий «хвост»."}},
    {"curve4_tip", {"The sharpest: the window stays wide almost to the end.", "Найрізкіша: вікно лишається широким майже до кінця.", "Самая резкая: окно остаётся широким почти до конца."}},
    {"curve5_tip", {"Smooth S-shape, closest to the classic genie effect.", "Плавна S-подібна, найближча до класичного ефекту «джина».", "Плавная S-образная, ближе всего к классическому эффекту «джинна»."}},
    {"curve6_tip", {"Bend along a circular arc, bulging sides.", "Вигин по дузі кола, опуклі боки.", "Изгиб по дуге окружности, выпуклые бока."}},
    {"curve7_tip", {"Wavy, springy sides.", "Хвилясті, пружні боки.", "Волнистые, пружинящие бока."}},
    {"curve8_tip", {"Smooth S-curve, softer at both ends.", "Плавна S-крива, м'якша на обох кінцях.", "Плавная S-кривая, мягче на обоих концах."}},

    {"motion0_tip", {"The same speed all the way.", "Однакова швидкість усю дорогу.", "Одинаковая скорость всю дорогу."}},
    {"motion1_tip", {"Starts slowly and speeds up, as if sucked into the panel.", "Рушає повільно й розганяється, ніби його засмоктує в панель.", "Трогается медленно и разгоняется, будто его засасывает в панель."}},
    {"motion2_tip", {"Speeds up gently and slows down before the end.", "М'яко розганяється й пригальмовує перед кінцем.", "Мягко разгоняется и притормаживает перед концом."}},
    {"motion3_tip", {"Leaves quickly and arrives softly.", "Швидко рушає й м'яко прибуває.", "Быстро трогается и мягко прибывает."}},
    {"motion4_tip", {"A small pull back first, then the flight.", "Спершу невеликий замах назад, потім політ.", "Сначала небольшой замах назад, затем полёт."}},
};
// clang-format on

// Description for a key, or an empty string when there is none.
inline QString lampTip(const QByteArray& key, int lang)
{
    const QByteArray tipKey = key + "_tip";
    for (const LampText& t : kTexts) {
        if (qstrcmp(t.key, tipKey.constData()) == 0) {
            return QString::fromUtf8(t.text[lang]);
        }
    }
    return QString();
}

inline QString lampText(const char* key, int lang)
{
    for (const LampText& t : kTexts) {
        if (qstrcmp(t.key, key) == 0) {
            return QString::fromUtf8(t.text[lang]);
        }
    }
    return QString::fromLatin1(key);
}
