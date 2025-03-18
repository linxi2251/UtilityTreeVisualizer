cmake_minimum_required(VERSION 3.16)

project(UtilityTreeVisualizer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appUtilityTreeVisualizer
    main.cpp
)

qt_add_resources(appUtilityTreeVisualizer "icons"
    PREFIX "/"
    FILES
    icons/system.png
    icons/folder.png
    icons/generic.png
    icons/water.png
    icons/telecom.png
    icons/pipeline.png
    icons/gas.png
    icons/pipe.png
    icons/point.png
    icons/area.png
    icons/feature.png
    icons/default.png
)

qt_add_qml_module(appUtilityTreeVisualizer
    URI UtilityTreeVisualizer
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES
        treeitem.cpp
        treeitem.h
        treemodel.cpp
        treemodel.h
    DEPENDENCIES
            QtQuick

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appUtilityTreeVisualizer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appUtilityTreeVisualizer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appUtilityTreeVisualizer
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appUtilityTreeVisualizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
