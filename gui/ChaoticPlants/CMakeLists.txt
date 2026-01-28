cmake_minimum_required(VERSION 3.16)

project(ChaoticPlants VERSION 0.1 LANGUAGES CXX)

include_directories(
    ../../src
    ../../src/Customer
    ../../src/Greenhouse
    ../../src/Staff
    ../../src/Nursery
)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(
    ../../src
    ../../src/Customer
    ../../src/Greenhouse
    ../../src/Staff
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Collect all source files from backend
file(GLOB_RECURSE CUSTOMER_SOURCES "../../src/Customer/*.cpp")
file(GLOB_RECURSE GREENHOUSE_SOURCES "../../src/Greenhouse/*.cpp")
file(GLOB_RECURSE STAFF_SOURCES "../../src/Staff/*.cpp")
file(GLOB_RECURSE NURSERY_SOURCES "../../src/Nursery/*.cpp")

# Exclude any main.cpp files and MOC-generated files from backend sources
list(FILTER CUSTOMER_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER GREENHOUSE_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER STAFF_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER NURSERY_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER CUSTOMER_SOURCES EXCLUDE REGEX ".*moc_.*\\.cpp$")
list(FILTER GREENHOUSE_SOURCES EXCLUDE REGEX ".*moc_.*\\.cpp$")
list(FILTER STAFF_SOURCES EXCLUDE REGEX ".*moc_.*\\.cpp$")
list(FILTER NURSERY_SOURCES EXCLUDE REGEX ".*moc_.*\\.cpp$")

set(PROJECT_SOURCES
        main.cpp
        nursery.cpp
        nursery.h
        nursery.ui
        ${CUSTOMER_SOURCES}
        ${GREENHOUSE_SOURCES}
        ${STAFF_SOURCES}
        ${NURSERY_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChaoticPlants
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ChaoticPlants APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ChaoticPlants SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ChaoticPlants
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ChaoticPlants PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChaoticPlants)
endif()
set_target_properties(ChaoticPlants PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChaoticPlants
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChaoticPlants)
endif()
