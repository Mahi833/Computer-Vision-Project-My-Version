cmake_minimum_required(VERSION 3.10)
project(VideoSnapshotExtractor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(video_snapshot_extractor video_snapshot_extractor.cpp)
target_link_libraries(video_snapshot_extractor ${OpenCV_LIBS})
