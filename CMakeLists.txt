cmake_minimum_required(VERSION 3.10)
project(cpp_multithreading_optimization)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 컴파일러 플래그 설정
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()

# 스레드 라이브러리 찾기
find_package(Threads REQUIRED)

# 소스 파일들
set(SOURCES
    src/main.cpp
    src/thread_pool.cpp
    src/benchmark.cpp
)

# 헤더 파일들
set(HEADERS
    include/thread_pool.h
    include/benchmark.h
)

# 실행 파일 생성
add_executable(multithreading_tests ${SOURCES} ${HEADERS})

# 헤더 디렉토리 추가
target_include_directories(multithreading_tests PRIVATE include)

# 스레드 라이브러리 링크
target_link_libraries(multithreading_tests Threads::Threads)

# 디버그 정보 추가
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(multithreading_tests PRIVATE DEBUG)
endif() 