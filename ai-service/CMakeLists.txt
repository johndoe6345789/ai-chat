## @brief nextra-ai — AI chat microservice.
cmake_minimum_required(VERSION 3.20)
project(nextra-ai VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

nextra_service(
    NAME nextra-ai
    DIRS
        ai-chat/backend
)
