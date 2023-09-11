#include "Timer.hpp"

#include <iostream>
#include <cassert>

using namespace task;

namespace {
	// Test fixtures
	struct test_callback
	{
		int cb_count{ 0 };

		void operator()()
		{
			++cb_count;
		}
	};
	struct test_callback2
	{
		int cb_count{ 0 };

		void operator()()
		{
			++cb_count;
			std::cout << "Scheduled Action: " << cb_count << std::endl;
		}
	};

	bool test_basic_one_shot()
	{
		test_callback on_expiry;
		timer t1{ std::chrono::milliseconds(5), false, std::ref(on_expiry) };
		t1.start();
		t1.stop();
		assert(on_expiry.cb_count == 1);
		return true;
	}

	bool test_one_shot_with_delay_and_cb()
	{
		test_callback on_expiry;
		test_callback on_start;
		timer t1{ std::chrono::milliseconds(5), false, std::ref(on_expiry) };
		t1.start(std::chrono::milliseconds(5), std::ref(on_start));
		t1.stop();
		assert(on_start.cb_count == 1);
		assert(on_expiry.cb_count == 1);
		return true;
	}

	bool test_periodic()
	{
		test_callback on_start;
		test_callback on_expiry;
		test_callback2 on_period;
		timer t1{ std::chrono::milliseconds(5), true, std::ref(on_expiry) };
		t1.start(std::chrono::milliseconds(0), std::ref(on_start), std::ref(on_period));
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		t1.stop();
		assert(on_start.cb_count == 1);
		assert(on_expiry.cb_count == 1);
		std::cout << "period count " << on_period.cb_count << std::endl;
		assert(on_period.cb_count > 1);
		return true;
	}
}
int main()
{
	std::cout << "test_basic_one_shot(): " << (test_basic_one_shot() ? "passed" : "failed") << std::endl;
	
	std::cout << "test_one_shot_with_delay_and_cb(): " << (test_one_shot_with_delay_and_cb() ? "passed" : "failed") << std::endl;

	std::cout << "test_periodic(): " << (test_periodic() ? "passed" : "failed") << std::endl;
	return 0;
}