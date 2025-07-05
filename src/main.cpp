#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include "thread_pool.h"
#include "benchmark.h"

// 기본 스레드 생성/소멸 테스트
void test_basic_threading() {
    std::cout << "\n=== 기본 스레드 생성/소멸 테스트 ===" << std::endl;
    
    auto create_destroy_threads = []() {
        std::thread t([]() {
            // 간단한 작업
            volatile int sum = 0;
            for (int i = 0; i < 1000; ++i) {
                sum += i;
            }
        });
        t.join();
    };
    
    auto result = Benchmark::run("스레드 생성/소멸", create_destroy_threads, 1000);
    Benchmark::print_results({result});
}

// 스레드 풀 테스트
void test_thread_pool() {
    std::cout << "\n=== 스레드 풀 테스트 ===" << std::endl;
    
    auto thread_pool_test = []() {
        ThreadPool pool(4);
        std::vector<std::future<int>> futures;
        
        for (int i = 0; i < 100; ++i) {
            futures.emplace_back(
                pool.enqueue([i]() {
                    // 간단한 계산 작업
                    int result = 0;
                    for (int j = 0; j < 1000; ++j) {
                        result += i * j;
                    }
                    return result;
                })
            );
        }
        
        // 모든 작업 완료 대기
        for (auto& future : futures) {
            future.get();
        }
    };
    
    auto result = Benchmark::run("스레드 풀 작업", thread_pool_test, 100);
    Benchmark::print_results({result});
}

// 동기화 기법 비교 테스트
void test_synchronization() {
    std::cout << "\n=== 동기화 기법 비교 테스트 ===" << std::endl;
    
    std::vector<std::pair<std::string, std::function<void()>>> tests;
    
    // Mutex 테스트
    tests.emplace_back("Mutex 동기화", []() {
        static std::mutex mtx;
        static int counter = 0;
        
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([]() {
                for (int j = 0; j < 1000; ++j) {
                    std::lock_guard<std::mutex> lock(mtx);
                    counter++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    });
    
    // Atomic 테스트
    tests.emplace_back("Atomic 동기화", []() {
        static std::atomic<int> counter{0};
        
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([]() {
                for (int j = 0; j < 1000; ++j) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    });
    
    auto results = Benchmark::compare(tests, 100);
    Benchmark::print_results(results);
}

// 작업 분할 전략 테스트
void test_work_distribution() {
    std::cout << "\n=== 작업 분할 전략 테스트 ===" << std::endl;
    
    const int total_work = 1000000;
    const int num_threads = 4;
    
    std::vector<std::pair<std::string, std::function<void()>>> tests;
    
    // 정적 분할
    tests.emplace_back("정적 분할", [total_work, num_threads]() {
        std::vector<std::thread> threads;
        int work_per_thread = total_work / num_threads;
        
        for (int i = 0; i < num_threads; ++i) {
            int start = i * work_per_thread;
            int end = (i == num_threads - 1) ? total_work : (i + 1) * work_per_thread;
            
            threads.emplace_back([start, end]() {
                volatile int sum = 0;
                for (int j = start; j < end; ++j) {
                    sum += j;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    });
    
    // 동적 분할 (간단한 구현)
    tests.emplace_back("동적 분할", [total_work, num_threads]() {
        std::atomic<int> current_work{0};
        std::vector<std::thread> threads;
        
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&current_work, total_work]() {
                volatile int sum = 0;
                while (true) {
                    int work = current_work.fetch_add(1000, std::memory_order_relaxed);
                    if (work >= total_work) break;
                    
                    int end = std::min(work + 1000, total_work);
                    for (int j = work; j < end; ++j) {
                        sum += j;
                    }
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    });
    
    auto results = Benchmark::compare(tests, 50);
    Benchmark::print_results(results);
}

int main() {
    std::cout << "C++ 멀티 스레딩 최적화 테스트 시작" << std::endl;
    std::cout << "CPU 코어 수: " << std::thread::hardware_concurrency() << std::endl;
    
    try {
        test_basic_threading();
        test_thread_pool();
        test_synchronization();
        test_work_distribution();
        
        std::cout << "\n모든 테스트 완료!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "오류 발생: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 