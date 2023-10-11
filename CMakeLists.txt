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

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

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

target_include_directories(${PCPLUGIN} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

# Request C++17
target_compile_features(${PCPLUGIN} PRIVATE cxx_std_17)

# link qt libraries
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Concurrent)

# link hdps and data plugins
set(MV_LINK_PATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib")
set(PLUGIN_LINK_PATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,lib,Plugins>")
set(MV_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(MV_LINK_LIBRARY "${MV_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}MV_Public${MV_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${MV_LINK_SUFFIX}") 

target_link_libraries(${PCPLUGIN} PRIVATE "${MV_LINK_LIBRARY}")
target_link_libraries(${PCPLUGIN} PRIVATE "${POINTDATA_LINK_LIBRARY}")

install(TARGETS ${PCPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${PCPLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${MV_INSTALL_DIR}/debug/ManiVault Studio.exe","${MV_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
