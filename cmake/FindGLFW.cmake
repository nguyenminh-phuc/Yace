include(FindPackageHandleStandardArgs)

if (WIN32)
   find_path(GLFW_INCLUDE_DIR
      NAMES
         GLFW/glfw3.h
      PATHS
         ${PROJECT_SOURCE_DIR}/extlibs/include
         ${GLFW_LOCATION}/include
         $ENV{GLFW_LOCATION}/include)

   find_library(GLFW_LIBRARY
      NAMES
         glfw3
      PATHS
         ${PROJECT_SOURCE_DIR}/extlibs/libs-msvc/${ARCHITECTURE}
         ${GLFW_LOCATION}/lib
         $ENV{GLFW_LOCATION}/lib)

#[[
elseif (${CMAKE_HOST_UNIX})
   find_path(GLFW_INCLUDE_DIR
      NAMES
         GLFW/glfw3.h
      PATHS
         ${GLFW_LOCATION}/include
         $ENV{GLFW_LOCATION}/include
         /usr/include
         /usr/local/include
         /sw/include
         /opt/local/include
         NO_DEFAULT_PATH)

   find_library(GLFW_LIBRARY
      NAMES
         glfw3
      PATHS
         ${GLFW_LOCATION}/lib
         $ENV{GLFW_LOCATION}/lib
         /usr/lib64
         /usr/lib
         /usr/local/lib64
         /usr/local/lib
         /sw/lib
         /opt/local/lib
         /usr/lib/x86_64-linux-gnu
         NO_DEFAULT_PATH)
]]

endif()

find_package_handle_standard_args(GLFW
   GLFW_INCLUDE_DIR
   GLFW_LIBRARY)

if(GLFW_FOUND)
   set(GLFW_LIBRARIES ${GLFW_LIBRARY})
   set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
endif()