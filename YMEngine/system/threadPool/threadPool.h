#pragma once
#include "../singleton/singleton.h"
namespace ym
{

    class ThreadPool : public Singleton<ThreadPool> {
        // Singleton<ThreadPool> が instance を作れるように
        friend class Singleton<ThreadPool>;

    protected:
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
            : stop_(false)
        {
            for (size_t i = 0; i < numThreads; ++i) {
                workers_.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            condVar_.wait(lock, [this] {
                                return stop_ || !tasks_.empty();
                                });
                            if (stop_ && tasks_.empty()) return;
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        busyCount_++;  // ★ task 開始時に ++
                        task();
                        busyCount_--;  // ★ task 終了時に --
                    }
                    });
            }
        }

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                stop_ = true;
            }
            condVar_.notify_all();
            for (auto &worker : workers_) {
                if (worker.joinable())
                    worker.join();
            }
        }

    public:
        // タスク追加。future で完了待ちができる
        template<typename F, typename... Args>
        auto enqueue(F &&f, Args&&... args)
            -> std::future<std::invoke_result_t<F, Args...>>
        {
            using return_type = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (stop_)
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                tasks_.emplace([task] { (*task)(); });
            }
            condVar_.notify_one();
            return res;
        }

        size_t GetThreadCount() const
        {
            return workers_.size();
        }
        int GetBusyThreadCount() const
        {
            return busyCount_.load();  // ★ atomicなので load() を使う
        }

    private:
        std::vector<std::thread>           workers_;
        std::queue<std::function<void()>>  tasks_;
        std::mutex                         mutex_;
        std::condition_variable            condVar_;
        bool                               stop_;
        std::atomic<int>                   busyCount_{ 0 };
    };
}