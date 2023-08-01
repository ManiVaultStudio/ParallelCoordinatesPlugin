cmake_minimum_required(VERSION 3.17)

set(PCPLUGIN "ParallelCoordinatesPlugin")

PROJECT(${PCPLUGIN})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus /W3")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 6.3.1 COMPONENTS Widgets WebEngineWidgets Concurrent REQUIRED)

set(PLUGIN
	src/ParallelCoordinatesPlugin.h
	src/ParallelCoordinatesPlugin.cpp
)

set(ACTIONS
	src/SettingsAction.h
	src/SettingsAction.cpp
	src/ClampAction.h
	src/ClampAction.cpp
	src/DimensionSelectionAction.h
	src/DimensionSelectionAction.cpp
)

set(WIDGETS
	src/ParallelCoordinatesWidget.h
	src/ParallelCoordinatesWidget.cpp
)

set(WEB
	res/parcoords/parcoords.html
	res/parcoords/parcoords.tools.js
	res/jslibs/d3.parcoords.css
	res/jslibs/d3.parcoords.js
	res/jslibs/d3.svg.multibrush.js
	res/jslibs/d3.v3.min.js
	res/jslibs/qwebchannel.tools.js
	res/jslibs/styles.css
)

set(AUX
	res/parcoords_resources.qrc
	src/ParallelCoordinatesPlugin.json
)

qt6_add_resources(RESOURCE_FILES res/parcoords_resources.qrc)

set(SOURCES ${PLUGIN} ${ACTIONS} ${WIDGETS})

source_group(Plugin FILES ${PLUGIN})
source_group(Actions FILES ${ACTIONS})
source_group(Widget FILES ${WIDGETS})
source_group(Web FILES ${WEB})
source_group(Aux FILES ${AUX})

add_library(${PCPLUGIN} SHARED ${SOURCES} ${WEB} ${AUX} ${RESOURCE_FILES})

target_include_directories(${PCPLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

# Request C++17
target_compile_features(${PCPLUGIN} PRIVATE cxx_std_17)

# link qt libraries
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Concurrent)

# link hdps and data plugins
set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(PLUGIN_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,lib,Plugins>")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${HDPS_LINK_SUFFIX}") 

target_link_libraries(${PCPLUGIN} PRIVATE "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PCPLUGIN} PRIVATE "${POINTDATA_LINK_LIBRARY}")

install(TARGETS ${PCPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${PCPLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
endif()
