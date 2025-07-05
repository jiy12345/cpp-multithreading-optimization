#include "benchmark.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Benchmark::Result Benchmark::run(const std::string& name, 
                                std::function<void()> func, 
                                size_t iterations) {
    Result result;
    result.name = name;
    result.iterations = iterations;
    
    // 워밍업
    for (size_t i = 0; i < 10; ++i) {
        func();
    }
    
    // 실제 벤치마크
    auto start = Clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    auto end = Clock::now();
    
    result.duration = std::chrono::duration_cast<Duration>(end - start);
    result.throughput = calculate_throughput(result.duration, iterations);
    
    return result;
}

std::vector<Benchmark::Result> Benchmark::compare(
    const std::vector<std::pair<std::string, std::function<void()>>>& tests,
    size_t iterations) {
    
    std::vector<Result> results;
    results.reserve(tests.size());
    
    for (const auto& test : tests) {
        results.push_back(run(test.first, test.second, iterations));
    }
    
    return results;
}

void Benchmark::print_results(const std::vector<Result>& results) {
    if (results.empty()) return;
    
    // 헤더 출력
    std::cout << std::setw(20) << "테스트명" 
              << std::setw(15) << "시간 (ms)" 
              << std::setw(15) << "반복 횟수"
              << std::setw(15) << "처리량 (ops/s)" 
              << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    
    // 결과 출력
    for (const auto& result : results) {
        double ms = std::chrono::duration<double, std::milli>(result.duration).count();
        
        std::cout << std::setw(20) << std::left << result.name
                  << std::setw(15) << std::fixed << std::setprecision(2) << ms
                  << std::setw(15) << result.iterations
                  << std::setw(15) << std::fixed << std::setprecision(0) << result.throughput
                  << std::endl;
    }
    
    // 최고 성능 찾기
    auto best = std::min_element(results.begin(), results.end(),
        [](const Result& a, const Result& b) {
            return a.duration < b.duration;
        });
    
    if (best != results.end()) {
        std::cout << "\n최고 성능: " << best->name 
                  << " (" << std::chrono::duration<double, std::milli>(best->duration).count() 
                  << " ms)" << std::endl;
    }
    
    std::cout << std::endl;
}

double Benchmark::calculate_throughput(const Duration& duration, size_t iterations) {
    double seconds = std::chrono::duration<double>(duration).count();
    return seconds > 0 ? iterations / seconds : 0.0;
} 