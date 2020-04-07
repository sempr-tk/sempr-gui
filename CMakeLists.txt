cmake_minimum_required(VERSION 3.5)

project(sempr-gui VERSION 0.1.0)

add_definitions(-DCEREAL_THREAD_SAFE=1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


add_definitions(-std=c++11 -Wall -Wextra -pedantic -Wnon-virtual-dtor)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(sempr REQUIRED sempr-core)
include_directories(${sempr_INCLUDE_DIRS})
link_directories(${sempr_LIBRARY_DIRS})


find_package(Qt5 COMPONENTS Core Widgets Quick QuickWidgets Location QuickControls2 REQUIRED)
include_directories(${Qt5_INCLUDE_DIRS})
link_directories(${Qt5_LIBRARY_DIRS})

include_directories(src)

set(GUI_SRC
    src/ECModel.cpp
    src/ModelEntry.cpp
    src/DirectConnection.cpp
    src/DirectConnectionNode.cpp
    src/DirectConnectionBuilder.cpp
    src/GeoMapWidget.cpp
    src/SemprGui.cpp
    src/RawComponentWidget.cpp
    src/RoleNameProxyModel.cpp
    src/TripleContainerWidget.cpp
    src/TriplePropertyMapWidget.cpp
    src/TripleVectorWidget.cpp
    src/StackedColumnsProxyModel.cpp
    src/UniqueFilterProxyModel.cpp
    src/UsefulWidget.cpp
    src/main.cpp
    ui/main.ui
    qrc/resources.qrc
)


add_executable(main ${GUI_SRC})
target_link_libraries(main ${sempr_LIBRARIES} Qt5::Core Qt5::Widgets Qt5::Quick Qt5::QuickWidgets Qt5::QuickControls2 Qt5::Location Threads::Threads)
