cmake_minimum_required(VERSION 3.5)
project(wxWidgetsApp)

# Enable C++17 (or change based on your needs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include wxWidgets settings
include(${wxWidgets_USE_FILE})

# Add your executable
add_executable(wxWidgetsApp src/main.cpp)

# Link wxWidgets libraries
target_link_libraries(wxWidgetsApp ${wxWidgets_LIBRARIES})

# Include wxWidgets headers explicitly
target_include_directories(wxWidgetsApp PRIVATE ${wxWidgets_INCLUDE_DIRS})

# Add required compile definitions
add_definitions(${wxWidgets_DEFINITIONS})

