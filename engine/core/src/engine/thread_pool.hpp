#ifndef ENTROPY8_ENGINE_THREAD_POOL_HPP
#define ENTROPY8_ENGINE_THREAD_POOL_HPP

#include <cstddef>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace entropy8 {

class ThreadPool {
public:
	explicit ThreadPool(std::size_t num_threads = 0);
	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

	std::size_t size() const { return workers_.size(); }

private:
	std::vector<std::thread> workers_;
	std::queue<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::condition_variable cond_;
	bool stop_{false};
};

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
	using return_type = typename std::result_of<F(Args...)>::type;
	auto task = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	std::future<return_type> res = task->get_future();
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (stop_) return res;
		tasks_.emplace([task]() { (*task)(); });
	}
	cond_.notify_one();
	return res;
}

} // namespace entropy8

#endif
