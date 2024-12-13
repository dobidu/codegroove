cmake_minimum_required(VERSION 3.22)

# Adding a directory with the library/application name as a subfolder of the
# include folder is a good practice. It helps avoid name clashes later on.
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/codegroove")

set(FORMATS Standalone VST3)

# Adds a plugin target (that's basically what the Projucer does).
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME LAViD # change this
    IS_SYNTH FALSE # may change this
    NEEDS_MIDI_INPUT FALSE # may change this
    NEEDS_MIDI_OUTPUT FALSE # may change this
    PLUGIN_MANUFACTURER_CODE PLMC # change this
    PLUGIN_CODE PLCO # change this
    FORMATS "${FORMATS}" # may change this
    PRODUCT_NAME "CodeGroove" # change this
)

target_link_libraries("CodeGroove" 
    PRIVATE 
        cpr::cpr
)

# Configure Python
set (Python_ROOT_DIR "/usr")
set(Python_USE_STATIC_LIBS OFF)
find_package (Python REQUIRED Interpreter Development.Embed)

message(STATUS "Python version: ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}.${Python_VERSION_PATCH}")
message(STATUS "Python Root: ${Python_ROOT_DIR}")
message(STATUS "Python Executable: ${Python_EXECUTABLE}")
message(STATUS "Python Include Dirs: ${Python_INCLUDE_DIRS}")
message(STATUS "Python Libraries: ${Python_LIBRARIES}")

# Generate a binary target of the python packages
set (ADDITIONAL_IGNORED_PYTHON_PATTERNS "email" "curses" "dbm" "html" "http" "unittest" "tomllib" "wsgiref" "xmlrpc")
set (ADDITIONAL_PYTHON_PACKAGES "numpy" "pedalboard" "pydub")
set (PYTHON_STANDARD_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.zip")
set (PYTHON_PACKAGES_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/../modules/site-packages.zip")

add_custom_target (
    ${PROJECT_NAME}_stdlib
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Script.py
        -b ${Python_ROOT_DIR} -o ${CMAKE_CURRENT_BINARY_DIR} -M ${Python_VERSION_MAJOR} -m ${Python_VERSION_MINOR}
        -x "\"${ADDITIONAL_IGNORED_PYTHON_PATTERNS}\"" -p "\"${ADDITIONAL_PYTHON_PACKAGES}\""
    BYPRODUCTS ${PYTHON_STANDARD_LIBRARY})

add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}_stdlib)

juce_add_binary_data (BinaryData SOURCES ${PYTHON_STANDARD_LIBRARY})
add_dependencies (BinaryData ${PROJECT_NAME}_stdlib)

# Sets the include directories of the plugin project.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${Python_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        include
)

# Sets the source files of the plugin project.
target_sources(${PROJECT_NAME}
    PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
)

juce_generate_juce_header(${PROJECT_NAME})

set (LTO_CONFIGURATION "")

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        Python::Python
        popsicle::juce_python
        BinaryData
        ${LTO_CONFIGURATION}
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        popsicle::juce_python_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        PYBIND11_DETAILED_ERROR_MESSAGES=1
)

# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall /W4)
    # Silences some deprecation warnings that (hopefully) will be fixed in a future release of JUCE.
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Xlinker -export-dynamic)
endif()

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)


