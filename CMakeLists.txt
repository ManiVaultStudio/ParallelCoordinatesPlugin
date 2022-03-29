cmake_minimum_required(VERSION 3.17)

set(PCPLUGIN "ParallelCoordinatesPlugin")

PROJECT(${PCPLUGIN})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Normalize the incoming install path
file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)
find_package(Qt5 COMPONENTS Widgets WebEngineWidgets Concurrent REQUIRED)

qt5_add_resources(RESOURCE_FILES res/parcoords_resources.qrc)

file(GLOB H src/*.h)
file(GLOB CPP src/*.cpp)
file(GLOB UIS src/*.ui)
file(GLOB WEB res/jslibs/*.js res/jslibs/*.css res/parcoords/*.js res/parcoords/*.html)
file(GLOB QRC src/*.qrc)

set(SOURCES ${H} ${CPP})
set(WEB ${WEB})
set(AUX ${QRC})

source_group(Plugin FILES ${SOURCES})
source_group(UI FILES ${UIS})
source_group(Web FILES ${WEB})
source_group(Aux FILES ${AUX})

QT5_WRAP_UI(UI_HEADERS ${UIS})

add_library(${PCPLUGIN} SHARED ${SOURCES} ${UIS} ${WEB} ${AUX} ${WEB} ${RESOURCE_FILES})

target_include_directories(${PCPLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

# Request C++17
target_compile_features(${PCPLUGIN} PRIVATE cxx_std_17)

target_link_libraries(${PCPLUGIN} PRIVATE Qt5::Widgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt5::WebEngineWidgets)
target_link_libraries(${PCPLUGIN} PRIVATE Qt5::Concurrent)
if(MSVC)
	set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
	set(LIB_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

target_link_libraries(${PCPLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_SUFFIX}")
target_link_libraries(${PCPLUGIN} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_SUFFIX}")

install(TARGETS ${PCPLUGIN}
   RUNTIME DESTINATION Plugins COMPONENT SHAREDLIB
   LIBRARY DESTINATION lib COMPONENT LINKLIB
   ARCHIVE DESTINATION lib COMPONENT LINKLIB
)

add_custom_command(TARGET ${PCPLUGIN} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

# Automatically set the debug environment (command + working directory) for MSVC in debug mode
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<$<CONFIG:DEBUG>:${INSTALL_DIR}/debug>)
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<$<CONFIG:DEBUG>:${INSTALL_DIR}/debug/HDPS.exe>)

# Automatically set the debug environment (command + working directory) for MSVC in release mode
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<$<CONFIG:RELEASE>:${INSTALL_DIR}/release>)
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<$<CONFIG:RELEASE>:${INSTALL_DIR}/release/HDPS.exe>)