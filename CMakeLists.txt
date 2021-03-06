cmake_minimum_required(VERSION 3.15...3.20)

# +---------------+
# | Project Setup |
# +---------------+

project(
  mylib
  VERSION 0.2.0
  DESCRIPTION "Example static library"
  LANGUAGES CXX)

message("Configuring ${PROJECT_NAME}...")

# Get default install locations
include(GNUInstallDirs)

# Create the library
add_library(${PROJECT_NAME} STATIC "")

# Add library alias
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set source files
target_sources(${PROJECT_NAME} PRIVATE src/my_lib.cpp src/utils.cpp)

# We require C++17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Set include dirs
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Check if the library is being used as the main project vs. via sub_directory()
# or similar. We'll use this to enable or disable tests, installation, etc.
if(NOT DEFINED ${PROJECT_NAME}_MAIN_PROJECT)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(${PROJECT_NAME}_MAIN_PROJECT ON)
  else()
    set(${PROJECT_NAME}_MAIN_PROJECT OFF)
  endif()

  message("${PROJECT_NAME}_MAIN_PROJECT: ${${PROJECT_NAME}_MAIN_PROJECT}")
endif()

# +--------------+
# | Dependencies |
# +--------------+

# Download Conan script if needed
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH
      SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON)
endif()

# Load the Conan script
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Configure Conan to grab our dependencies
conan_cmake_configure(REQUIRES spdlog/1.8.5 GENERATORS cmake_find_package)

# Do some settings stuff??
conan_cmake_autodetect(settings)

# Install the dependencies
conan_cmake_install(
  PATH_OR_REFERENCE
  .
  BUILD
  missing
  REMOTE
  conan-center
  SETTINGS
  ${settings})

# This is necessary otherwise find_package() doesn't look in the right place for
# the conan dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

# Find the Conan dependencies
find_package(spdlog REQUIRED)

# Link in the dependenices
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

# +---------+
# | Testing |
# +---------+

option(BUILD_${PROJECT_NAME}_TESTS "Build ${PROJECT_NAME} tests"
       ${${PROJECT_NAME}_MAIN_PROJECT})

message("BUILD_${PROJECT_NAME}_TESTS: ${BUILD_${PROJECT_NAME}_TESTS}")

include(CTest) # Provides the BUILD_TESTING option (ON by default)
if(((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) OR BUILD_${PROJECT_NAME}_TESTS)
   AND BUILD_TESTING)
  message("${PROJECT_NAME} tests will be built...")
  enable_testing()
  add_subdirectory(test)
endif()

# +----------------------+
# | Library Installation |
# +----------------------+

option(INSTALL_${PROJECT_NAME} "Enable installation of ${PROJECT_NAME} library"
       ${${PROJECT_NAME}_MAIN_PROJECT})

message("INSTALL_${PROJECT_NAME}: ${INSTALL_${PROJECT_NAME}}")

if(INSTALL_${PROJECT_NAME})
  message("Configuring library installation...")

  # Install locations are provided by GNUInstallDirs
  install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(
    EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # TODO: Properly implement CPack stuff for package creation
  # set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
  # include(CPack)

endif()
