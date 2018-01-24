if (BUILD_SHARED_LIBS)
   add_definitions(-DYACE_DLL)
endif()

include_directories("../../include")

file(GLOB TICTAC_SOURCES "*.hpp" "*.cpp")

add_executable(TicTac ${TICTAC_SOURCES})

target_link_libraries(TicTac Yace)

set_target_properties(TicTac PROPERTIES FOLDER "examples")

install(TARGETS TicTac DESTINATION ${INSTALL_DIR})