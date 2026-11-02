set(kcm_SRCS
    YetAnotherMagicLampEffectKCM.cc
    plugin.cc
)

kconfig_add_kcfg_files(kcm_SRCS
    ../YetAnotherMagicLampConfig.kcfgc
)

qt6_wrap_ui(kcm_SRCS YetAnotherMagicLampEffectKCM.ui)

qt6_add_dbus_interface(kcm_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_yetanothermagiclamp_config MODULE ${kcm_SRCS})

target_link_libraries(kwin_yetanothermagiclamp_config
    Qt6::Core
    Qt6::DBus
    Qt6::Gui
    Qt6::Widgets
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::ConfigWidgets
    KF6::KCMUtils
)

# Translations of the settings page, embedded as Qt resources (optional).
find_package(Qt6 QUIET COMPONENTS LinguistTools)
if(Qt6LinguistTools_FOUND)
    qt_add_lrelease(kwin_yetanothermagiclamp_config
        TS_FILES
            translations/kwin_yetanothermagiclamp_config_ru.ts
            translations/kwin_yetanothermagiclamp_config_uk.ts
        QM_FILES_OUTPUT_VARIABLE kcm_qm_files)
    qt_add_resources(kwin_yetanothermagiclamp_config kcm_translations
        PREFIX /i18n
        BASE ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${kcm_qm_files})
endif()

install(
    TARGETS
        kwin_yetanothermagiclamp_config

    DESTINATION
        ${KDE_INSTALL_QTPLUGINDIR}/kwin/effects/configs/
)
