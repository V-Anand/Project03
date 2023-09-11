#include "Timer.hpp"

#include <thread>
#include <exception>

using namespace task;

void timer::start(clock_type::duration const& delay)
{
	start_impl(delay, callback_type(), callback_type());
}

void timer::start(clock_type::duration const& delay, callback_type const& callback)
{
	start_impl(delay, callback, callback_type());
}

void timer::start(clock_type::duration const& delay, callback_type const& callback, callback_type const& sched_action)
{
	start_impl(delay, callback, sched_action);
}

void timer::start_impl(clock_type::duration const& delay, callback_type const& on_start_cb, callback_type const& sched_action)
{
	std::promise<void> flag;
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_state != timer_state::Initial)
			return;

		auto const now = clock_type::now();

		const bool is_repeating = m_repeating;

		const auto is_stopped = [this]() {
			return this->is_stopped();
		};

		const clock_type::duration period = m_period;

		std::shared_future<void> flag_set = flag.get_future().share();

		std::thread task([flag_set, is_stopped, delay, now, on_start_cb, period, sched_action, is_repeating]() {
			flag_set.wait();

			if (delay > clock_type::duration(0)) {
				std::this_thread::sleep_until(now + delay);
			}

			if (on_start_cb) {
				on_start_cb();
			}

			while (is_repeating && !is_stopped()) {
				std::this_thread::sleep_for(period);
			    sched_action();
			}

			});

		m_state = timer_state::Active;
		m_timer_task = std::move(task);
	}
	flag.set_value();
}

bool timer::is_repeating() const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	return m_repeating;
}

bool timer::is_stopped() const
{
	std::lock_guard<std::mutex> lock(m_mutex);

	return (m_state == timer_state::Done);
}

void timer::stop()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_state = timer_state::Done;
	}

	m_timer_task.join();

	if (m_expiry_cb) {
		m_expiry_cb();
	}
}