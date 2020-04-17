cmake_minimum_required(VERSION 3.5)

project(sempr-gui VERSION 0.1.0)

add_definitions(-DCEREAL_THREAD_SAFE=1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


add_definitions(-std=c++11 -Wall -Wextra -pedantic -Wnon-virtual-dtor)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# sempr, for the components
pkg_check_modules(sempr REQUIRED sempr-core)
include_directories(${sempr_INCLUDE_DIRS})
link_directories(${sempr_LIBRARY_DIRS})

# zmqpp - c++ wrapper for ZeroMQ
find_package(ZeroMQPP REQUIRED)
include_directories(${ZeroMQPP_INCLUDE_DIRS})
link_directories(${ZeroMQPP_LIBRARY_DIRS})
message("libs: ${ZeroMQPP_LIBRARIES}")

# apparently, the above is missing zmq itself...
pkg_check_modules(zmq REQUIRED libzmq)
include_directories(${zmq_INCLUDE_DIRS})
link_directories(${zmq_LIBRARY_DIRS})

# qt stuff
find_package(Qt5 COMPONENTS Core Widgets Quick QuickWidgets Location QuickControls2 REQUIRED)
include_directories(${Qt5_INCLUDE_DIRS})
link_directories(${Qt5_LIBRARY_DIRS})

include_directories(src)

set(GUI_SRC
    src/AbstractInterface.cpp
    src/ECModel.cpp
    src/ModelEntry.cpp
    src/ColoredBranchTreeView.cpp
    src/DirectConnection.cpp
    src/DirectConnectionNode.cpp
    src/DirectConnectionBuilder.cpp
    src/FlattenTreeProxyModel.cpp
    src/GeometryFilterProxyModel.cpp
    src/GeoMapWidget.cpp
    src/GeosQCoordinateTranform.cpp
    src/SemprGui.cpp
    src/RawComponentWidget.cpp
    src/RoleNameProxyModel.cpp
    src/TCPConnectionClient.cpp
    src/TCPConnectionServer.cpp
    src/TripleContainerWidget.cpp
    src/TriplePropertyMapWidget.cpp
    src/TripleVectorWidget.cpp
    src/StackedColumnsProxyModel.cpp
    src/UniqueFilterProxyModel.cpp
    src/UsefulWidget.cpp
    ui/main.ui
    ui/raw_component.ui
    ui/triplecontainer.ui
    ui/triplepropertymap.ui
    ui/triplevector.ui
    ui/geomap.qml
    ui/geomap.ui
    ui/CoordinateDelegate.qml
    ui/PolygonDelegate.qml
    qrc/resources.qrc
)

add_library(sempr-gui SHARED ${GUI_SRC})
target_link_libraries(sempr-gui
    ${sempr_LIBRARIES} ${zmq_LIBRARIES} ${ZeroMQPP_LIBRARIES}
    Qt5::Core Qt5::Widgets Qt5::Quick Qt5::QuickWidgets Qt5::QuickControls2
    Qt5::Location Threads::Threads)


add_executable(main src/main.cpp)
target_link_libraries(main sempr-gui)

add_executable(example_server src/ExampleServer.cpp)
target_link_libraries(example_server sempr-gui)

add_executable(example_client src/ExampleClient.cpp)
target_link_libraries(example_client sempr-gui)
