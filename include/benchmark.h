#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <functional>

class Benchmark {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::nanoseconds;
    
    struct Result {
        std::string name;
        Duration duration;
        size_t iterations;
        double throughput; // 작업/초
    };

    // 단일 벤치마크 실행
    static Result run(const std::string& name, 
                     std::function<void()> func, 
                     size_t iterations = 1000);

    // 여러 벤치마크 비교 실행
    static std::vector<Result> compare(
        const std::vector<std::pair<std::string, std::function<void()>>>& tests,
        size_t iterations = 1000);

    // 결과 출력
    static void print_results(const std::vector<Result>& results);
    
    // 통계 계산
    static double calculate_throughput(const Duration& duration, size_t iterations);
}; 