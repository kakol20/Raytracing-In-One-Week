# CMakeList.txt : CMake project for Raytracing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Raytracing")

# set(Raytracing "Raytracing")

# Add source to this project's executable.
add_executable (Raytracing  
	"src/main.cpp" "src/managers/MainManager.cpp")

find_package(glad CONFIG REQUIRED)
target_link_libraries(Raytracing PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Raytracing PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(Raytracing PRIVATE imgui::imgui)

find_package(Stb REQUIRED)
target_include_directories(Raytracing PRIVATE ${Stb_INCLUDE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Raytracing PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
