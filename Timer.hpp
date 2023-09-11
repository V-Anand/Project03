#pragma once
#include <functional>
#include <chrono>
#include <future>
#include <thread>
#include <mutex>

namespace task {

	class timer {
	public:
		enum class timer_state {Initial, Active, Done};

		using callback_type = std::function<void()>;
		using clock_type = std::chrono::high_resolution_clock;

		template <typename Type, typename Period>
		timer(std::chrono::duration<Type, Period> const& period, bool is_repeating, callback_type const& on_expiry_cb)
			: m_mutex()
			, m_period(std::chrono::duration_cast<clock_type::duration>(period))
			, m_repeating(is_repeating)
			, m_expiry_cb(on_expiry_cb)
			, m_start()
			, m_state(timer_state::Initial)
			, m_timer_task()
		{
		}
		~timer() = default;

		// not copyable
		timer(timer const&) = delete;
		timer& operator=(timer const&) = delete;

		// not movable
		timer(timer&&) = delete;
		timer& operator=(timer&&) = delete;

		// start timer without a callback
		void start(clock_type::duration const& start_delay = std::chrono::milliseconds(0));

		// start timer with on_start callback
		void start(clock_type::duration const& start_delay, callback_type const& on_start_cb);

		// start timer with on_start callback and periodic actions
		void start(clock_type::duration const& start_delay, callback_type const& on_start_cb, callback_type const& scheduled_action);

		bool is_repeating() const;

		bool is_stopped() const;

		// stop timer
		void stop();

	private:
		// start timer implementation
		void start_impl(clock_type::duration const& start_delay, callback_type const& on_start_cb, callback_type const& sched_action);

		mutable std::mutex m_mutex;

		// timer period
		clock_type::duration m_period;

		// if true: repeating timer; if false: one-shot timer
		bool m_repeating;

		// callback on expiry
		callback_type m_expiry_cb;

		clock_type::time_point m_start;
		timer_state m_state;
		std::thread m_timer_task;
	};
}
