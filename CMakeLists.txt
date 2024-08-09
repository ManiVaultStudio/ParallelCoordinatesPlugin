cmake_minimum_required(VERSION 3.17)

option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)

# -----------------------------------------------------------------------------
# Project: Parallel Coordinates plugin
# -----------------------------------------------------------------------------
set(PCPLUGIN "ParallelCoordinatesPlugin")
PROJECT(${PCPLUGIN})

# -----------------------------------------------------------------------------
# CMake Options
# -----------------------------------------------------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus /W3")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets Concurrent REQUIRED)

find_package(ManiVault COMPONENTS Core PointData CONFIG)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${PCPLUGIN} SHARED ${SOURCES} ${WEB} ${AUX} ${RESOURCE_FILES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
target_include_directories(${PCPLUGIN} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${PCPLUGIN} PRIVATE cxx_std_17)

if(MV_UNITY_BUILD)
    set_target_properties(${PCPLUGIN} PROPERTIES UNITY_BUILD ON)
endif()

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
# link qt libraries
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Widgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt6::Concurrent)

target_link_libraries(${PCPLUGIN} PRIVATE ManiVault::Core)
target_link_libraries(${PCPLUGIN} PRIVATE ManiVault::PointData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${PCPLUGIN}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${PCPLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
)

# -----------------------------------------------------------------------------
# Misc
# -----------------------------------------------------------------------------
# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/debug,${ManiVault_INSTALL_DIR}/release>)
	set_property(TARGET ${PCPLUGIN} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/debug/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
