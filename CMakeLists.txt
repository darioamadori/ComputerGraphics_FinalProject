# 1)cambiare il cmakelist e cambiare i nomi del progetto
# 2) edit comnfiguration working directory e metti la Trott del progetto che stai facendo
# 3) caso disperato, andare in un progetto che renna copiare il make list e rifare 1 e 2
#########################################################
#                       Variables                       #
# Set them to match your SDK and libraries installation #
#########################################################
set($ENV{VULKAN_SDK} "/Users/darioamadori/VulkanSDK/1.3.275.0")      # Vulkan SDK
set(GLFW "/Users/darioamadori/VulkanSDK/libs/glfw-3.4.bin.MACOS")  # GLFW
set(GLM "/Users/darioamadori/VulkanSDK/libs/glm")                    # GLM
# List of directories to add to the include path:
# all headers in these directories can be included in the source code
# (Vulkan headers must be omitted from here)

list(APPEND INCLUDE_DIRS "${GLFW}/include" ${GLM} headers)
# List of libraries to link to the executable:
# all these libraries will be linked to the compiled executable
# (Vulkan library must be omitted from here)
list(APPEND LINK_LIBS "${GLFW}/lib-arm64/libglfw3.a")
# Name of the project, will be used as the target name for CMake build
# and will be the name of the executable
set(PROJ_NAME island)

# Name of the source file containing the main() function
set(MAIN island.cpp)
#########################################################
#                  CMake configuration                  #
# You shouldn't need to modify anything past this point #
#########################################################

# Find the CoreFoundation framework
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)

# Find the CoreGraphics framework
find_library(CORE_GRAPHICS_FRAMEWORK CoreGraphics)

# Find the IOKit framework
find_library(IOKIT_FRAMEWORK IOKit)

# Assuming your target is named island, link the frameworks

cmake_minimum_required(VERSION 3.22.3)
project(${PROJ_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${MAIN})
find_package(Vulkan REQUIRED)
find_library(COCOA_FRAMEWORK Cocoa)
target_link_libraries(${PROJ_NAME}
        "${COCOA_FRAMEWORK}"
        "${CORE_FOUNDATION_FRAMEWORK}"
        "${CORE_GRAPHICS_FRAMEWORK}"
        "${IOKIT_FRAMEWORK}"
        # other frameworks and libraries as needed
        )

foreach(dir IN LISTS Vulkan_INCLUDE_DIR INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PUBLIC ${dir})
endforeach()
foreach(lib IN LISTS Vulkan_LIBRARIES LINK_LIBS)
    target_link_libraries(${PROJECT_NAME} ${lib})
endforeach()

# Find the CoreFoundation framework
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)

# Find the CoreGraphics framework
find_library(CORE_GRAPHICS_FRAMEWORK CoreGraphics)

# Find the IOKit framework
find_library(IOKIT_FRAMEWORK IOKit)
cmake_minimum_required(VERSION 3.10)
project(island)

# Your other CMake configuration and add_executable() calls

# Find the frameworks
find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
find_library(CORE_GRAPHICS_FRAMEWORK CoreGraphics)
find_library(IOKIT_FRAMEWORK IOKit)

# Check if the frameworks were found
if(NOT CORE_FOUNDATION_FRAMEWORK)
    message(FATAL_ERROR "CoreFoundation framework not found")
endif()

if(NOT CORE_GRAPHICS_FRAMEWORK)
    message(FATAL_ERROR "CoreGraphics framework not found")
endif()

if(NOT IOKIT_FRAMEWORK)
    message(FATAL_ERROR "IOKit framework not found")
endif()

# Link the frameworks to your target (replace 'island' with your actual target name)
target_link_libraries(island
        "${CORE_FOUNDATION_FRAMEWORK}"
        "${CORE_GRAPHICS_FRAMEWORK}"
        "${IOKIT_FRAMEWORK}"
        )

# The rest of your CMake configuration


# Assuming your target is named island, link the frameworks
target_link_libraries(island
        "${CORE_FOUNDATION_FRAMEWORK}"
        "${CORE_GRAPHICS_FRAMEWORK}"
        "${IOKIT_FRAMEWORK}"
        # Add other frameworks and libraries your project needs
        )
