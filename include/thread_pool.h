#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    // 작업을 스레드 풀에 추가
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;

    // 모든 작업이 완료될 때까지 대기
    void wait_all();

private:
    // 스레드들이 실행할 함수
    void worker_thread();

    // 멤버 변수들
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    // 동기화를 위한 변수들
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
}; 