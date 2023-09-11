Timer Files
===========
Timer.hpp -- The "timer" class declaration
Timer.cpp -- The "timer" definition

main.cpp -- Unit tests

Use Cases
=========
1) Create one-shot timer with optional expiry callback.
2) Create periodic timer with optional expiry callback.
3) Ability to provide "start delay" in the timer::start() API call
4) Ability to schedule an action with defined period in the timer::start() API call
5) Ability to stop timer with timer::stop()
 

Unit tests included in main.cpp for the implementation.
