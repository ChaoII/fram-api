//
// Created by AC on 2023/12/7.
//

#pragma once

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>

using namespace std;


class ThreadPool {
public:
    inline explicit ThreadPool(unsigned short size = 4, bool auto_grow = true, int max_thread_num = 16) {
        init_size_ = size;
        auto_grow_ = auto_grow;
        max_thread_num_ = max_thread_num;
        addThread(size);
    }

    inline ~ThreadPool() {
        run_ = false;
        task_cv_.notify_all(); // 唤醒所有线程执行
        for (thread &thread: pool_) {
            //thread.detach(); // 让线程“自生自灭”
            if (thread.joinable())
                thread.join(); // 等待任务结束， 前提：线程一定会执行完
        }
    }

public:
    template<class F, class... Args>
    auto commit(F &&f, Args &&... args) -> future<decltype(f(args...))> {
        if (!run_) {
            throw runtime_error("commit on ThreadPool is stopped.");
        }
        using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
        auto task = make_shared<packaged_task<RetType()>>(
                bind(forward<F>(f), forward<Args>(args)...)); // 把函数入口及参数,打包(绑定)
        future<RetType> future = task->get_future();
        {   // 添加任务到队列
            // 对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
            lock_guard<mutex> lock{lock_};
            // push(Task{...}) 放到队列后面
            tasks_.emplace([task]() {
                (*task)();
            });
        }
        if (auto_grow_) {
            if (idl_thread_num_ < 1 && pool_.size() < max_thread_num_)
                addThread(1);
        }
        // 唤醒一个线程执行
        task_cv_.notify_one();
        return future;
    }

    //空闲线程数量
    int idlCount() { return idl_thread_num_; }

    //线程数量
    size_t thrCount() { return pool_.size(); }

private:
    //添加指定数量的线程
    void addThread(unsigned short size) {
        if (!auto_grow_) {
            if (!run_)    // stoped ??
                throw runtime_error("Grow on ThreadPool is stopped.");
            unique_lock<mutex> lockGrow{lock_grow_}; //自动增长锁
        }
        for (; pool_.size() < max_thread_num_ && size > 0; --size) {   //增加线程数量,但不超过 预定义数量 max_thread_num_
            pool_.emplace_back([this] { //工作线程函数
                //防止 run_==false 时立即结束,此时任务队列可能不为空
                while (true) {
                    Task task; // 获取一个待执行的 task
                    {
                        // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                        unique_lock<mutex> lock{lock_};
                        task_cv_.wait(lock, [this] { // wait 直到有 task, 或需要停止
                            return !run_ || !tasks_.empty();
                        });
                        if (!run_ && tasks_.empty())
                            return;
                        idl_thread_num_--;
                        task = std::move(tasks_.front()); // 按先进先出从队列取一个 task
                        tasks_.pop();
                    }
                    task();//执行任务
                    if (!auto_grow_) {
                        if (idl_thread_num_ > 0 && pool_.size() > init_size_) //支持自动释放空闲线程,避免峰值过后大量空闲线程
                            return;
                    }
                    {
                        unique_lock<mutex> lock{lock_};
                        idl_thread_num_++;
                    }
                }
            });
            {
                unique_lock<mutex> lock{lock_};
                idl_thread_num_++;
            }
        }
    }

private:
    using Task = function<void()>; //定义类型
    //初始化线程数量
    unsigned short init_size_;
    // 线程池最大容量,应尽量设小一点
    int max_thread_num_;
    // 线程池是否可以自动增长
    bool auto_grow_;
    vector<thread> pool_;           //线程池
    queue<Task> tasks_;             //任务队列
    mutex lock_;                    //任务队列同步锁
    mutex lock_grow_;               //线程池增长同步锁
    condition_variable task_cv_;    //条件阻塞
    atomic<bool> run_{true};            //线程池是否执行
    atomic<int> idl_thread_num_{0};     //空闲线程数量
};



