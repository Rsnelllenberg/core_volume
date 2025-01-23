cmake_minimum_required(VERSION 3.17)

set(VOLUMEDATA "VolumeData")
PROJECT(${VOLUMEDATA})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

file(GLOB JSON src/*.json)
set(AUX ${JSON})

set(VOLUME_DATA_HEADERS 
    src/Volume.h
    src/VolumeData.h
    src/Volumes.h
    src/Common.h
    ${CMAKE_CURRENT_BINARY_DIR}/volumedata_export.h
)

set(PLUGIN_MOC_HEADERS
    src/VolumeData.h
)

set(VOLUME_DATA_SOURCES 
    src/Volume.cpp
    src/VolumeData.cpp
    src/Volumes.cpp
)

set(ACTIONS_SOURCES 
    src/InfoAction.h
    src/InfoAction.cpp
)

set(PLUGIN_GRAPHICS
    src/Size3D.h
    src/Size3D.cpp
)

set(ACTIONS_SOURCES 
    src/InfoAction.h
    src/InfoAction.cpp
)

source_group(Actions FILES ${ACTIONS_SOURCES})
source_group(Plugin FILES ${VOLUME_DATA_HEADERS} ${VOLUME_DATA_SOURCES} ${PLUGIN_GRAPHICS})
source_group(Aux FILES ${AUX})

add_library(${VOLUMEDATA} SHARED ${VOLUME_DATA_HEADERS} ${VOLUME_DATA_SOURCES} ${PLUGIN_GRAPHICS} ${ACTIONS_SOURCES} ${AUX})

add_library(ManiVault::${VOLUMEDATA} ALIAS ${VOLUMEDATA})

qt_wrap_cpp(VOLUMEDATA_MOC ${PLUGIN_MOC_HEADERS} TARGET ${VOLUMEDATA})
target_sources(${VOLUMEDATA} PRIVATE ${VOLUMEDATA_MOC})

target_include_directories(${VOLUMEDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${VOLUMEDATA} PRIVATE cxx_std_17)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${VOLUMEDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME 
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND VOLUME_DATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${VOLUMEDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${VOLUMEDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${VOLUMEDATA} PRIVATE ${MV_PUBLIC_LIB})
target_link_libraries(${VOLUMEDATA} PRIVATE PointData)
target_link_libraries(${VOLUMEDATA} PRIVATE ClusterData)

## Use AVX if enabled and available
#mv_check_and_set_AVX(${VOLUMEDATA} ${MV_USE_AVX})

set_target_properties(${VOLUMEDATA} PROPERTIES PUBLIC_HEADER "${VOLUME_DATA_HEADERS}")

if(MV_UNITY_BUILD)
    set_target_properties(${VOLUMEDATA} PROPERTIES UNITY_BUILD ON)
    set_source_files_properties(${VOLUMEDATA_MOC} PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)
endif()

install(TARGETS ${VOLUMEDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${VOLUMEDATA}/" COMPONENT HEADERS
)

add_custom_command(TARGET ${VOLUMEDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)
