include(FindPackageHandleStandardArgs)

if (WIN32)
   find_path(GLEW_INCLUDE_DIR
      NAMES
         GL/glew.h
      PATHS
         ${PROJECT_SOURCE_DIR}/extlibs/include
         ${GLEW_LOCATION}/include
         $ENV{GLEW_LOCATION}/include)

   find_library(GLEW_LIBRARY
      NAMES
         glew32
      PATHS
         ${PROJECT_SOURCE_DIR}/extlibs/libs-msvc/${ARCHITECTURE}
         ${GLEW_LOCATION}/lib
         $ENV{GLEW_LOCATION}/lib)

#[[
elseif (${CMAKE_HOST_UNIX})
   find_path(GLEW_INCLUDE_DIR
      NAMES
         GL/glew.h
      PATHS
         ${GLEW_LOCATION}/include
         $ENV{GLEW_LOCATION}/include
         /usr/include
         /usr/local/include
         /sw/include
         /opt/local/include
         NO_DEFAULT_PATH)

   find_library(GLEW_LIBRARY
      NAMES
         GLEW
      PATHS
         ${GLEW_LOCATION}/lib
         $ENV{GLEW_LOCATION}/lib
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

find_package_handle_standard_args(GLEW
   GLEW_INCLUDE_DIR
   GLEW_LIBRARY)

if(GLEW_FOUND)
   set(GLEW_LIBRARIES ${GLEW_LIBRARY})
   set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
endif()