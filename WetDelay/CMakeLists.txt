cmake_minimum_required(VERSION 3.14.0)

option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "../vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(WetDelay
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 1.0.0.0 
    DESCRIPTION "WetDelay VST 3 Plug-in"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(WetDelay
    source/version.h
    source/wetdelaycids.h
    source/wetdelayprocessor.h
    source/wetdelayprocessor.cpp
    source/wetdelaycontroller.h
    source/wetdelaycontroller.cpp
    source/wetdelayentry.cpp
    source/delaybuffer.h
    source/delaybuffer.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(WetDelay
        PRIVATE
            resource/wetdelayeditor.uidesc
    )
    target_link_libraries(WetDelay
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(WetDelay
        RESOURCES
            "resource/wetdelayeditor.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (WetDelay
    RESOURCES
        resource/B4C7B10DBFB05B3C9A71DCAD9E9F2BD9_snapshot.png
        resource/B4C7B10DBFB05B3C9A71DCAD9E9F2BD9_snapshot_2.0x.png
)

target_link_libraries(WetDelay
    PRIVATE
        sdk
)

smtg_target_configure_version_file(WetDelay)

if(SMTG_MAC)
    smtg_target_set_bundle(WetDelay
        BUNDLE_IDENTIFIER org.yonie.vst3.wetdelay
        COMPANY_NAME "Yonie"
    )
    smtg_target_set_debug_executable(WetDelay
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(WetDelay PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WetDelay)

        smtg_target_set_debug_executable(WetDelay
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
