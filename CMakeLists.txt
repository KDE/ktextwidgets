cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.49.0") # handled by release scripts
set(KF5_DEP_VERSION "5.49.0") # handled by release scripts
project(KTextWidgets VERSION ${KF5_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.49.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include(GenerateExportHeader)
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
set(REQUIRED_QT_VERSION 5.8.0)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Widgets)

find_package(Qt5 OPTIONAL_COMPONENTS TextToSpeech)
if (NOT Qt5TextToSpeech_FOUND)
    message(STATUS "Qt5TextToSpeech not found, speech feature will be disabled")
else()
    add_definitions(-DHAVE_SPEECH)
endif()

find_package(KF5Completion ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5ConfigWidgets ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5IconThemes ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Service ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Sonnet ${KF5_DEP_VERSION} REQUIRED)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

add_definitions(-DTRANSLATION_DOMAIN=\"ktextwidgets5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
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
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
       )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
