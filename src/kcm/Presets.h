/*
 * Presets and translations for the settings page.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <QByteArray>
#include <QString>

// One complete look. Durations of 0 mean "KWin's default speed".
struct LampPreset {
    const char* key; // translation key of the name; description key is key + "_desc"
    int stretch, unminimizeStretch, squash, bump;
    int maxBumpDistance, gridResolution;
    double shapeFactor;
    int shapeCurve; // 0 Linear .. 8 Bezier
    int motion; // 0 even, 1 accelerate, 2 smooth, 3 decelerate, 4 anticipate
    int spring, fade, wave, neckCurve; // percent
};

// Index 0 in the list is "Custom"; presets start at 1 (same as the config value).
inline constexpr LampPreset kPresets[] = {
    { "classic", 0, 0, 0, 0, 0, 30, 0.20, 2, 0, 0, 0, 0, 0 },
    { "genie", 200, 200, 260, 0, 0, 40, 0.35, 5, 1, 0, 0, 0, 0 },
    { "soft", 300, 300, 380, 220, 20, 40, 0.25, 5, 2, 0, 25, 0, 0 },
    { "fast", 110, 110, 150, 0, 0, 24, 0.30, 2, 1, 0, 0, 0, 0 },
    { "springy", 220, 200, 260, 180, 30, 36, 0.25, 2, 2, 60, 0, 0, 0 },
    { "jelly", 260, 240, 300, 0, 0, 48, 0.25, 4, 4, 100, 0, 30, 0 },
    { "ghost", 260, 260, 320, 0, 0, 30, 0.25, 5, 3, 0, 100, 0, 0 },
    { "silk", 300, 300, 360, 0, 0, 60, 0.30, 5, 2, 0, 0, 55, 0 },
    { "whirl", 220, 220, 280, 0, 0, 48, 0.30, 1, 1, 0, 0, 0, 70 },
    { "smoke", 280, 260, 340, 0, 0, 48, 0.30, 5, 3, 0, 70, 25, -30 },
};
inline constexpr int kPresetCount = int(sizeof(kPresets) / sizeof(kPresets[0]));

// Order of columns in every LampText::text below. Add a new language by
// appending one code here AND one extra string at the same position in every
// entry.
inline constexpr const char* kLanguageCodes[] = { "en", "de", "ru", "uk" };
inline constexpr int kLanguageCount = int(sizeof(kLanguageCodes) / sizeof(kLanguageCodes[0]));

struct LampText {
    const char* key;
    const char* text[kLanguageCount];
};

// clang-format off
inline constexpr LampText kTexts[] = {
    {"preset",        {"Preset:", "Vorlage:", "Пресет:", "Пресет:"}},
    {"language",      {"Language:", "Sprache:", "Язык:", "Мова:"}},
    {"system",        {"System default", "Systemstandard", "Как в системе", "Як у системі"}},
    {"custom",        {"Custom", "Benutzerdefiniert", "Свой", "Власний"}},
    {"custom_desc",   {"Your own combination of the settings below.",
                       "Ihre eigene Kombination der Einstellungen unten.",
                       "Ваше собственное сочетание настроек ниже.",
                       "Ваше власне поєднання налаштувань нижче."}},
    {"classic",       {"Classic", "Klassisch", "Классический", "Класичний"}},
    {"classic_desc",  {"The original Yet Another Magic Lamp.",
                       "Das originale Yet Another Magic Lamp.",
                       "Оригинальный Yet Another Magic Lamp.",
                       "Оригінальний Yet Another Magic Lamp."}},
    {"genie",         {"Genie (macOS-like)", "Genie (wie macOS)", "Джинн (как в macOS)", "Джин (як у macOS)"}},
    {"genie_desc",    {"Narrow neck, the window speeds up as it is sucked into the panel.",
                       "Schmaler Hals, das Fenster beschleunigt, während es in die Leiste gesogen wird.",
                       "Узкая шея, окно разгоняется, когда его затягивает в панель.",
                       "Вузька шия, вікно розганяється, коли його затягує в панель."}},
    {"soft",          {"Soft", "Weich", "Мягкий", "М'який"}},
    {"soft_desc",     {"Slow and smooth, with a gentle lift and a little fading.",
                       "Langsam und sanft, mit leichtem Anheben und etwas Ausblenden.",
                       "Медленно и плавно, с лёгким подъёмом и немного гаснет.",
                       "Повільно й плавно, з легким підйомом і трохи згасає."}},
    {"fast",          {"Fast", "Schnell", "Быстрый", "Швидкий"}},
    {"fast_desc",     {"Quick and snappy, for everyday work.",
                       "Schnell und knackig, für den Alltag.",
                       "Быстро и чётко, для ежедневной работы.",
                       "Швидко й чітко, для щоденної роботи."}},
    {"springy",       {"Springy", "Federnd", "Пружинящий", "Пружний"}},
    {"springy_desc",  {"Windows bounce a little when they come back.",
                       "Fenster federn leicht, wenn sie zurückkehren.",
                       "Окна немного пружинят, когда возвращаются.",
                       "Вікна трохи пружинять, коли повертаються."}},
    {"jelly",         {"Jelly", "Wackelpudding", "Желе", "Желе"}},
    {"jelly_desc",    {"A pull back before the jump, a wobbly neck and a strong wobble on return.",
                       "Ein Ausholen vor dem Sprung, ein wackelnder Hals und starkes Wackeln bei der Rückkehr.",
                       "Замах перед прыжком, волнистая шея и сильное дрожание при возвращении.",
                       "Замах перед стрибком, хвиляста шия і сильне тремтіння при поверненні."}},
    {"ghost",         {"Ghost", "Geist", "Призрак", "Привид"}},
    {"ghost_desc",    {"The window melts away as it reaches the panel.",
                       "Das Fenster löst sich auf, wenn es die Leiste erreicht.",
                       "Окно тает, когда долетает до панели.",
                       "Вікно тане, коли долітає до панелі."}},
    {"silk",          {"Silk", "Seide", "Шёлк", "Шовк"}},
    {"silk_desc",     {"The neck ripples like fabric in the wind.",
                       "Der Hals wellt sich wie Stoff im Wind.",
                       "Шея колышется, как ткань на ветру.",
                       "Шия хвилюється, як тканина на вітрі."}},
    {"whirl",         {"Whirlwind", "Wirbelwind", "Вихрь", "Вихор"}},
    {"whirl_desc",    {"The neck curves in an arc towards the icon.",
                       "Der Hals krümmt sich im Bogen zum Symbol.",
                       "Шея изгибается дугой в сторону иконки.",
                       "Шия вигинається дугою в бік іконки."}},
    {"smoke",         {"Smoke", "Rauch", "Дымка", "Димок"}},
    {"smoke_desc",    {"A curved, rippling neck that fades like smoke.",
                       "Ein gekrümmter, wellender Hals, der wie Rauch verweht.",
                       "Изогнутая волнистая шея, которая рассеивается, как дым.",
                       "Вигнута хвиляста шия, що розвіюється, як дим."}},

    {"sec_shape",     {"Shape", "Form", "Форма", "Форма"}},
    {"sec_motion",    {"Motion", "Bewegung", "Движение", "Рух"}},
    {"sec_timing",    {"Timing", "Dauer", "Длительность", "Тривалість"}},
    {"sec_advanced",  {"Advanced", "Erweitert", "Дополнительно", "Додатково"}},

    {"ShapeCurve",          {"Neck shape:", "Halsform:", "Форма шеи:", "Форма шиї:"}},
    {"InitialShapeFactor",  {"Initial neck width:", "Anfängliche Halsbreite:", "Начальная ширина шеи:", "Початкова ширина шиї:"}},
    {"NeckCurve",           {"Neck bend:", "Halskrümmung:", "Изгиб шеи:", "Вигин шиї:"}},
    {"Wave",                {"Ripple:", "Welle:", "Волна:", "Хвиля:"}},
    {"Fade",                {"Fade out:", "Ausblenden:", "Затухание:", "Згасання:"}},
    {"Motion",              {"Speed profile:", "Geschwindigkeitsprofil:", "Характер движения:", "Характер руху:"}},
    {"Spring",              {"Spring on restore:", "Federung beim Wiederherstellen:", "Пружинистость при разворачивании:", "Пружність при розгортанні:"}},
    {"Duration",            {"Overall duration:", "Gesamtdauer:", "Общая длительность:", "Загальна тривалість:"}},
    {"StretchDuration",     {"Stretch:", "Strecken:", "Растяжение:", "Розтягування:"}},
    {"UnminimizeStretchDuration", {"Stretch on restore:", "Strecken beim Wiederherstellen:", "Растяжение при разворачивании:", "Розтягування при розгортанні:"}},
    {"SquashDuration",      {"Squash:", "Stauchen:", "Сжатие:", "Стискання:"}},
    {"BumpDuration",        {"Lift:", "Anheben:", "Подъём:", "Підйом:"}},
    {"MaxBumpDistance",     {"Lift height:", "Anhebehöhe:", "Высота подъёма:", "Висота підйому:"}},
    {"GridResolution",      {"Mesh detail:", "Netzdetail:", "Детализация сетки:", "Деталізація сітки:"}},

    {"curve0", {"Linear", "Linear", "Линейная", "Лінійна"}},
    {"curve1", {"Quadratic", "Quadratisch", "Квадратичная", "Квадратична"}},
    {"curve2", {"Cubic", "Kubisch", "Кубическая", "Кубічна"}},
    {"curve3", {"Quartic", "Vierten Grades", "Четвёртой степени", "Четвертого степеня"}},
    {"curve4", {"Quintic", "Fünften Grades", "Пятой степени", "П'ятого степеня"}},
    {"curve5", {"Sine", "Sinus", "Синусоида", "Синусоїда"}},
    {"curve6", {"Circular", "Kreisförmig", "Круговая", "Колова"}},
    {"curve7", {"Bounce", "Sprünge", "Прыжки", "Стрибки"}},
    {"curve8", {"Bezier", "Bézier", "Безье", "Безьє"}},

    {"motion0", {"Even (classic)", "Gleichmäßig (klassisch)", "Равномерно (классически)", "Рівномірно (класично)"}},
    {"motion1", {"Accelerate into the panel", "In die Leiste beschleunigen", "С разгоном в панель", "З розгоном у панель"}},
    {"motion2", {"Smooth start and end", "Sanfter Start und sanftes Ende", "Плавно в начале и в конце", "Плавно на початку й наприкінці"}},
    {"motion3", {"Fast start, gentle arrival", "Schneller Start, sanfte Ankunft", "Быстрый старт, мягкое прибытие", "Швидкий старт, м'яке прибуття"}},
    {"motion4", {"Anticipate (pull back first)", "Mit Ausholen", "С замахом", "Із замахом"}},

    {"default", {"Default", "Standard", "По умолчанию", "Типово"}},
    {"off",     {"Off", "Aus", "Выкл.", "Вимк."}},
    {"ms",      {" ms", " ms", " мс", " мс"}},
    {"px",      {" px", " px", " пкс", " пкс"}},
    {"hint",    {"Press Apply, then minimize any window to see it.",
                 "Klicken Sie auf „Anwenden“ und minimieren Sie ein Fenster, um es zu sehen.",
                 "Нажмите «Применить» и сверните любое окно, чтобы увидеть.",
                 "Натисніть «Застосувати» і згорніть будь-яке вікно, щоб побачити."}},

    // Descriptions: "<key>_tip" is the tooltip of a row (label and field) or
    // of a combo box item; the selected curve/motion is also shown below it.
    {"ShapeCurve_tip",         {"Outline of the window's neck while it is pulled into the panel.",
                                "Umriss des Fensterhalses, während es in die Leiste gezogen wird.",
                                "Очертание «шеи» окна, пока его втягивает в панель.",
                                "Обрис «шиї» вікна, поки його втягує в панель."}},
    {"InitialShapeFactor_tip", {"How narrowed the window already is when the animation starts: 0 starts from the full window, higher values start with a funnel.",
                                "Wie stark das Fenster zu Beginn der Animation bereits verengt ist: 0 beginnt beim vollen Fenster, höhere Werte starten direkt mit einem Trichter.",
                                "Насколько окно уже сужено в начале анимации: 0 — начинает с полного окна, большие значения — сразу с «воронкой».",
                                "Наскільки вікно вже звужене на початку анімації: 0 — починає з повного вікна, більші значення — одразу з «лійкою»."}},
    {"NeckCurve_tip",          {"Bends the neck into an arc instead of a straight funnel; negative values bend it the other way. 0 = straight.",
                                "Krümmt den Hals zu einem Bogen statt zu einem geraden Trichter; negative Werte krümmen in die andere Richtung. 0 = gerade.",
                                "Изгибает шею дугой вместо прямой «воронки»; отрицательные значения — в другую сторону. 0 — прямая.",
                                "Вигинає шию дугою замість прямої «лійки»; від'ємні значення — в інший бік. 0 — пряма."}},
    {"Wave_tip",               {"Cloth-like ripple running along the neck. 0 = off.",
                                "Stoffartige Welle, die den Hals entlangläuft. 0 = aus.",
                                "Волна вдоль шеи, как ткань на ветру. 0 — выключено.",
                                "Хвиля вздовж шиї, як тканина на вітрі. 0 — вимкнено."}},
    {"Fade_tip",               {"How much the window fades out as it reaches the panel. 0 = stays opaque, 100 % = disappears completely.",
                                "Wie stark das Fenster ausblendet, während es die Leiste erreicht. 0 = bleibt deckend, 100 % = verschwindet vollständig.",
                                "Насколько окно исчезает, подлетая к панели. 0 — остаётся непрозрачным, 100 % — исчезает полностью.",
                                "Наскільки вікно зникає, підлітаючи до панелі. 0 — лишається непрозорим, 100 % — зникає повністю."}},
    {"Motion_tip",             {"How the speed changes during the flight. On restore it plays in reverse.",
                                "Wie sich die Geschwindigkeit während des Fluges ändert. Beim Wiederherstellen rückwärts.",
                                "Как меняется скорость во время полёта. При разворачивании — в обратном порядке.",
                                "Як змінюється швидкість під час польоту. При розгортанні — у зворотному порядку."}},
    {"Spring_tip",             {"The window wobbles a little when it comes back from the panel. 0 = off.",
                                "Das Fenster federt leicht, wenn es aus der Leiste zurückkehrt. 0 = aus.",
                                "Окно немного пружинит, когда возвращается из панели. 0 — выключено.",
                                "Вікно трохи пружинить, коли повертається з панелі. 0 — вимкнено."}},
    {"Duration_tip",           {"Length of the whole animation. \"Default\" uses KWin's animation speed and the phases below.",
                                "Länge der gesamten Animation. „Standard“ nutzt die Animationsgeschwindigkeit von KWin und die Phasen unten.",
                                "Длительность всей анимации. «По умолчанию» — по скорости анимаций KWin и фазам ниже.",
                                "Тривалість усієї анімації. «Типово» — за швидкістю анімацій KWin і фазами нижче."}},
    {"StretchDuration_tip",    {"Phase in which the window stretches into the neck when minimizing.",
                                "Phase, in der sich das Fenster beim Minimieren in den Hals streckt.",
                                "Фаза, когда окно вытягивается в «шею» при сворачивании.",
                                "Фаза, коли вікно витягується в «шию» під час згортання."}},
    {"UnminimizeStretchDuration_tip", {"The same stretch phase when the window is restored.",
                                "Dieselbe Streckphase beim Wiederherstellen des Fensters.",
                                "Та же фаза растяжения, когда окно разворачивается.",
                                "Та сама фаза розтягування, коли вікно розгортається."}},
    {"SquashDuration_tip",     {"Phase in which the stretched window is squeezed into the icon.",
                                "Phase, in der das gestreckte Fenster in das Symbol gestaucht wird.",
                                "Фаза, когда вытянутое окно сжимается в значок.",
                                "Фаза, коли витягнуте вікно стискається в значок."}},
    {"BumpDuration_tip",       {"Phase in which the window lifts off the panel edge before flying.",
                                "Phase, in der sich das Fenster vor dem Flug vom Leistenrand abhebt.",
                                "Фаза, когда окно поднимается над краем панели перед полётом.",
                                "Фаза, коли вікно піднімається над краєм панелі перед польотом."}},
    {"MaxBumpDistance_tip",    {"How high the window lifts above the panel at most.",
                                "Wie hoch sich das Fenster maximal über die Leiste hebt.",
                                "Насколько максимально окно поднимается над панелью.",
                                "Наскільки найвище вікно піднімається над панеллю."}},
    {"GridResolution_tip",     {"How finely the window is split for bending. Higher = smoother curves, slightly more GPU work.",
                                "Wie fein das Fenster zum Verbiegen unterteilt wird. Höher = glattere Kurven, etwas mehr GPU-Last.",
                                "Насколько мелко окно делится для изгиба. Больше — более плавные кривые, чуть больше работы для GPU.",
                                "Наскільки дрібно вікно ділиться для вигину. Більше — плавніші криві, трохи більше роботи для GPU."}},

    {"curve0_tip", {"Straight wedge, like a trapezoid.", "Gerader Keil, wie ein Trapez.", "Ровный клин, как трапеция.", "Рівний клин, як трапеція."}},
    {"curve1_tip", {"Soft bend.", "Sanfter Bogen.", "Мягкий изгиб.", "М'який вигин."}},
    {"curve2_tip", {"Noticeable neck: wide on top, narrows sharply near the icon.",
                    "Deutlicher Hals: oben breit, verengt sich beim Symbol stark.",
                    "Заметная «шея»: вверху широкое, у значка резко сужается.",
                    "Помітна «шия»: угорі широке, біля значка різко звужується."}},
    {"curve3_tip", {"Even sharper: wide body, thin tail.", "Noch schärfer: breiter Körper, dünner Schwanz.", "Ещё резче: широкое «тело», тонкий «хвост».", "Ще різкіше: широке «тіло», тонкий «хвіст»."}},
    {"curve4_tip", {"The sharpest: the window stays wide almost to the end.", "Am schärfsten: Das Fenster bleibt fast bis zum Ende breit.", "Самая резкая: окно остаётся широким почти до конца.", "Найрізкіша: вікно лишається широким майже до кінця."}},
    {"curve5_tip", {"Smooth S-shape, closest to the classic genie effect.", "Sanfte S-Form, am nächsten am klassischen Genie-Effekt.", "Плавная S-образная, ближе всего к классическому эффекту «джинна».", "Плавна S-подібна, найближча до класичного ефекту «джина»."}},
    {"curve6_tip", {"Bend along a circular arc, bulging sides.", "Bogen entlang eines Kreises, gewölbte Seiten.", "Изгиб по дуге окружности, выпуклые бока.", "Вигин по дузі кола, опуклі боки."}},
    {"curve7_tip", {"Wavy, springy sides.", "Wellige, federnde Seiten.", "Волнистые, пружинящие бока.", "Хвилясті, пружні боки."}},
    {"curve8_tip", {"Smooth S-curve, softer at both ends.", "Sanfte S-Kurve, weicher an beiden Enden.", "Плавная S-кривая, мягче на обоих концах.", "Плавна S-крива, м'якша на обох кінцях."}},

    {"motion0_tip", {"The same speed all the way.", "Durchgehend dieselbe Geschwindigkeit.", "Одинаковая скорость всю дорогу.", "Однакова швидкість усю дорогу."}},
    {"motion1_tip", {"Starts slowly and speeds up, as if sucked into the panel.", "Beginnt langsam und beschleunigt, als würde es in die Leiste gesogen.", "Трогается медленно и разгоняется, будто его засасывает в панель.", "Рушає повільно й розганяється, ніби його засмоктує в панель."}},
    {"motion2_tip", {"Speeds up gently and slows down before the end.", "Beschleunigt sanft und bremst vor dem Ende ab.", "Мягко разгоняется и притормаживает перед концом.", "М'яко розганяється й пригальмовує перед кінцем."}},
    {"motion3_tip", {"Leaves quickly and arrives softly.", "Startet schnell und kommt sanft an.", "Быстро трогается и мягко прибывает.", "Швидко рушає й м'яко прибуває."}},
    {"motion4_tip", {"A small pull back first, then the flight.", "Zuerst ein kleines Ausholen, dann der Flug.", "Сначала небольшой замах назад, затем полёт.", "Спершу невеликий замах назад, потім політ."}},
};
// clang-format on

inline int langIndex(const QByteArray& code)
{
    for (int i = 0; i < kLanguageCount; ++i) {
        if (code == kLanguageCodes[i]) {
            return i;
        }
    }
    return 0; // first entry (English) is the fallback
}

// Description for a key, or an empty string when there is none.
inline QString lampTip(const QByteArray& key, const QByteArray& lang)
{
    const QByteArray tipKey = key + "_tip";
    const int idx = langIndex(lang);
    for (const LampText& t : kTexts) {
        if (qstrcmp(t.key, tipKey.constData()) == 0) {
            return QString::fromUtf8(t.text[idx]);
        }
    }
    return QString();
}

inline QString lampText(const char* key, const QByteArray& lang)
{
    const int idx = langIndex(lang);
    for (const LampText& t : kTexts) {
        if (qstrcmp(t.key, key) == 0) {
            return QString::fromUtf8(t.text[idx]);
        }
    }
    return QString::fromLatin1(key);
}
