# CMakeList.txt : CMake project for good-bot-runner, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("good-bot-runner")

# Add source to this project's executable.
add_executable (good-bot-runner "good-bot-runner.cpp" "good-bot-runner.h" "PtyHandler.h" "PtyHandler.cpp" )

target_link_libraries(good-bot-runner -lutil)

# TODO: Add tests and install targets if needed.
