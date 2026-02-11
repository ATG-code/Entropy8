#include "thread_pool.hpp"
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace entropy8 {

static std::size_t default_num_threads() {
#ifdef _WIN32
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors > 0 ? static_cast<std::size_t>(si.dwNumberOfProcessors) : 2u;
#else
	long n = sysconf(_SC_NPROCESSORS_ONLN);
	return n > 0 ? static_cast<std::size_t>(n) : 2u;
#endif
}

ThreadPool::ThreadPool(std::size_t num_threads) {
	std::size_t n = num_threads > 0 ? num_threads : default_num_threads();
	workers_.reserve(n);
	for (std::size_t i = 0; i < n; ++i) {
		workers_.emplace_back([this] {
			for (;;) {
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(mutex_);
					cond_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
					if (stop_ && tasks_.empty()) return;
					if (tasks_.empty()) continue;
					task = std::move(tasks_.front());
					tasks_.pop();
				}
				task();
			}
		});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		stop_ = true;
	}
	cond_.notify_all();
	for (auto& w : workers_) {
		if (w.joinable()) w.join();
	}
}

} // namespace entropy8
