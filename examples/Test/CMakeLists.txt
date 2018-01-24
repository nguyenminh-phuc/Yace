if (BUILD_SHARED_LIBS)
   add_definitions(-DYACE_DLL)
endif()

include_directories("../../include")

file(GLOB TEST_SOURCES "*.cpp")

add_executable(Test ${TEST_SOURCES})

target_link_libraries(Test Yace)

set_target_properties(Test PROPERTIES FOLDER "examples")

install(TARGETS Test DESTINATION ${INSTALL_DIR})
