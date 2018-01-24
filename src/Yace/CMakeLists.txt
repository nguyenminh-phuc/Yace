if (WIN32)
   find_package(OpenGL REQUIRED)
   find_package(GLEW REQUIRED)
   find_package(GLFW REQUIRED)
elseif (UNIX)
   find_package(PkgConfig REQUIRED)
   pkg_search_module(OpenGL REQUIRED gl)
   pkg_search_module(GLEW REQUIRED glew)
   pkg_search_module(GLFW REQUIRED glfw3)
endif()

add_definitions(-DGLFW_INCLUDE_GLCOREARB -DGLFW_DLL)
if (BUILD_SHARED_LIBS)
   add_definitions(-D_YACE_BUILD_DLL)
endif()

include_directories("../../include")
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

file(GLOB YACE_SOURCES "../../include/Yace/*.hpp" "*.cpp")

add_library(Yace ${YACE_SOURCES})

target_link_libraries(Yace ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES})

install(TARGETS Yace DESTINATION ${INSTALL_DIR})
if (WIN32)
   install(DIRECTORY "${CMAKE_SOURCE_DIR}/extlibs/libs-msvc/${ARCHITECTURE}/bin/" DESTINATION ${INSTALL_DIR})
endif()