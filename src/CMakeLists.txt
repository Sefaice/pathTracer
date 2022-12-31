cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name and version
project(pathTracer VERSION 0.2)
# configure version into header
configure_file(config.h.in config.h)

# add the executable
add_executable(pathTracer main.cpp)

find_package(embree 3.0 REQUIRED)
find_package(openimageio 2.3.14.0 REQUIRED)

include_directories(/opt/homebrew/Cellar/imath/3.1.5/include)

target_include_directories(pathTracer PUBLIC ${PROJECT_BINARY_DIR} ${OpenImageIO_INCLUDES})

### hard code .lib file for now
# mac
# target_link_libraries(pathTracer embree ${OpenImageIO_LIB_DIR}/libOpenImageIO.2.3.14.dylib ${OpenImageIO_LIB_DIR}/libOpenImageIO_Util.2.3.14.dylib)
# pc
target_link_libraries(pathTracer embree OpenImageIO::OpenImageIO OpenImageIO::OpenImageIO_Util)