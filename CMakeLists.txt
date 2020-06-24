cmake_minimum_required(VERSION 3.5)

project(sempr-gui VERSION 0.2.0)

add_definitions(-DCEREAL_THREAD_SAFE=1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# needed for dpkg-shlibdeps to find the libsempr-gui.so as a dependency of
# the example client/server
list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


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

# graphviz to layout the rete and explanation graphs
pkg_check_modules(CGRAPH REQUIRED libcgraph libgvc libcdt)
include_directories(${CGRAPH_INCLUDE_DIRS})
link_directories(${CGRAPH_LIBRARY_DIRS})

# qt stuff
find_package(Qt5 COMPONENTS Core Widgets Quick QuickWidgets Location QuickControls2 REQUIRED)
include_directories(${Qt5_INCLUDE_DIRS})
link_directories(${Qt5_LIBRARY_DIRS})

# boost filesystem, just to create a new folder for the database when needed
# (example server)
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(src)

set(GUI_SRC
    src/AbstractInterface.cpp
    src/AnyColumnFilterProxyModel.cpp
    src/ECModel.cpp
    src/ModelEntry.cpp
    src/ColoredBranchTreeView.cpp
    src/ComponentAdderWidget.cpp
    src/DirectConnection.cpp
    src/DirectConnectionNode.cpp
    src/DirectConnectionBuilder.cpp
    src/DragDropTabBar.cpp
    src/ExplanationToGraphVisitor.cpp
    src/ExplanationWidget.cpp
    src/FlattenTreeProxyModel.cpp
    src/GeometryFilterProxyModel.cpp
    src/GeoMapWidget.cpp
    src/GeosQCoordinateTranform.cpp
    src/GraphvizLayout.cpp
    src/ReteWidget.cpp
    src/RawComponentWidget.cpp
    src/GraphNodeItem.cpp
    src/GraphEdgeItem.cpp
    src/ReteVisualSerialization.cpp
    src/RoleNameProxyModel.cpp
    src/SemprGui.cpp
    src/SPARQLItem.cpp
    src/SPARQLWidget.cpp
    src/TCPConnectionClient.cpp
    src/TCPConnectionServer.cpp
    src/TextComponentWidget.cpp
    src/TripleContainerWidget.cpp
    src/TriplePropertyMapWidget.cpp
    src/TripleVectorWidget.cpp
    src/TripleLiveViewWidget.cpp
    src/StackedColumnsProxyModel.cpp
    src/UniqueFilterProxyModel.cpp
    src/UsefulWidget.cpp
    src/ZoomGraphicsView.cpp
    ui/main.ui
    ui/explanationwidget.ui
    ui/raw_component.ui
    ui/sparqlwidget.ui
    ui/textcomponent.ui
    ui/triplecontainer.ui
    ui/triplepropertymap.ui
    ui/triplevector.ui
    ui/componentadderwidget.ui
    ui/tripleliveviewwidget.ui
    ui/geomap.qml
    ui/geomap.ui
    ui/MapDelegates/CoordinateDelegate.qml
    ui/MapDelegates/PolygonDelegate.qml
    ui/MapDelegates/PointDelegate.qml
    qrc/resources.qrc
)

add_library(sempr-gui SHARED ${GUI_SRC})
target_link_libraries(sempr-gui
    ${sempr_LIBRARIES} ${zmq_LIBRARIES} ${ZeroMQPP_LIBRARIES} ${CGRAPH_LIBRARIES}
    Qt5::Core Qt5::Widgets Qt5::Quick Qt5::QuickWidgets Qt5::QuickControls2
    Qt5::Location Threads::Threads)
set_target_properties(sempr-gui PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})


add_executable(main src/main.cpp)
target_link_libraries(main sempr-gui)

set(server_name sempr-gui-example-server)
set(client_name sempr-gui-example-client)

add_executable(${server_name} src/ExampleServer.cpp)
target_link_libraries(${server_name} sempr-gui ${Boost_LIBRARIES})

add_executable(${client_name} src/ExampleClient.cpp)
target_link_libraries(${client_name} sempr-gui)


# configure pkg config
configure_file("sempr-gui.pc.in" "sempr-gui.pc" @ONLY)

# install stuff
install(
    TARGETS sempr-gui
    LIBRARY DESTINATION lib
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-gui.pc"
    DESTINATION lib/pkgconfig
)

install(
    DIRECTORY src/.
    DESTINATION include/sempr-gui
    FILES_MATCHING PATTERN *.hpp
)
install(
    TARGETS ${client_name} ${server_name}
    RUNTIME DESTINATION bin
)


# configure cpack
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI library for sempr, including an example client using a tcp connection to the sempr-core.")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nils Niemann <nils.niemann@dfki.de>")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")

include(CPack)
