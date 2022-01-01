cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.90.0") # handled by release scripts
set(KF_DEP_VERSION "5.90.0") # handled by release scripts
project(KTextWidgets VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.90.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(ECMAddQch)

ecm_setup_version(PROJECT
                  VARIABLE_PREFIX KTEXTWIDGETS
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ktextwidgets_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5TextWidgetsConfigVersion.cmake"
                  SOVERSION 5)

# Dependencies
set(REQUIRED_QT_VERSION 5.15.2)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets)

# No QTextToSpeech in Qt6.2
if (QT_MAJOR_VERSION STREQUAL "5")
    find_package(Qt5 OPTIONAL_COMPONENTS TextToSpeech)
endif()
if (NOT Qt5TextToSpeech_FOUND)
    message(STATUS "Qt5TextToSpeech not found, speech feature will be disabled")
else()
    add_definitions(-DHAVE_SPEECH)
endif()

find_package(KF5Completion ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Sonnet ${KF_DEP_VERSION} REQUIRED)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

option(BUILD_DESIGNERPLUGIN "Build plugin for Qt Designer" ON)
add_feature_info(DESIGNERPLUGIN ${BUILD_DESIGNERPLUGIN} "Build plugin for Qt Designer")

add_definitions(-DTRANSLATION_DOMAIN=\"ktextwidgets5\")
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055900)
ki18n_install(po)
add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5TextWidgets")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5TextWidgets_QCH
        FILE KF5TextWidgetsQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5TextWidgetsQchTargets.cmake\")")
endif()

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KF5TextWidgetsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KF5TextWidgetsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                             )

install(EXPORT KF5TextWidgetsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5TextWidgetsTargets.cmake NAMESPACE KF5:: )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KF5TextWidgetsConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/KF5TextWidgetsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
       )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ktextwidgets_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF} COMPONENT Devel
       )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
