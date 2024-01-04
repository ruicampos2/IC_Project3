cmake_minimum_required(VERSION 3.16)
project(opencv-example)

find_package(OpenCV REQUIRED)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pthread")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../opencv-bin)

add_executable (program2_dec program2_dec.cpp)
target_link_libraries (program2_dec ${OpenCV_LIBS})

add_executable (program2_enc program2_enc.cpp)
target_link_libraries (program2_enc ${OpenCV_LIBS})

add_executable (program3_dec program3_dec.cpp)
target_link_libraries (program3_dec ${OpenCV_LIBS})

add_executable (program3_enc program3_enc.cpp)
target_link_libraries (program3_enc ${OpenCV_LIBS})

add_executable (program4_dec_inter program4_dec_inter.cpp)
target_link_libraries (program4_dec_inter ${OpenCV_LIBS})

add_executable (program4_enc_inter program4_enc_inter.cpp)
target_link_libraries (program4_enc_inter ${OpenCV_LIBS})

add_executable (program4_dec_intra program4_dec_intra.cpp)
target_link_libraries (program4_dec_inter ${OpenCV_LIBS})

add_executable (program4_enc_intra program4_enc_intra.cpp)
target_link_libraries (program4_enc_intra ${OpenCV_LIBS})

