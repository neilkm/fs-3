#pragma once

#include <functional>
#include <list>
#include <vector>

struct Task {
    int ticksToExecute;
    std::function<void()> anon;
    bool repeat;
    int initialTicks; // For repeating tasks

    // Constructor for easy initialization
    Task(int tte, std::function<void()> func, bool rpt = false, int initTicks = 0)
        : ticksToExecute(tte), anon(func), repeat(rpt), initialTicks(initTicks) {}
};

class TaskQueue {
private:
    int maxEvents;
    std::list<Task> eventQueue;

public:
    // Constructor that sets the maximum number of allowed events
    TaskQueue(int n) : maxEvents(n) {}

    // Adds an immediate task with tte (ticksToExecute) of 0
    void call_immediate(std::function<void()> function);

    // Adds a task that will be executed after 'ticks' ticks
    void call_in(std::function<void()> function, int ticks);

    // Adds a repeating task that executes every 'ticks' ticks
    void call_every(std::function<void()> function, int ticks);

    // Main tick logic
    void handle_tick_event();

    // Clears the event queue
    void clean();
};
