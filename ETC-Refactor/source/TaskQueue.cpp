#include "TaskQueue.h"

// Adds an immediate task with tte (ticksToExecute) of 0
void TaskQueue::call_immediate(std::function<void()> function)
{
    if (eventQueue.size() < maxEvents)
    {
        eventQueue.emplace_back(0, function, false);
    }
    else
    {
        // Handle overflow: You can log an error or handle as needed
    }
}

// Adds a task that will be executed after 'ticks' ticks
void TaskQueue::call_in(std::function<void()> function, int ticks)
{
    if (eventQueue.size() < maxEvents)
    {
        eventQueue.emplace_back(ticks, function, false);
    }
    else
    {
        // Handle overflow
    }
}

// Adds a repeating task that executes every 'ticks' ticks
void TaskQueue::call_every(std::function<void()> function, int ticks)
{
    if (eventQueue.size() < maxEvents)
    {
        eventQueue.emplace_back(ticks, function, true, ticks);
    }
    else
    {
        // Handle overflow
    }
}

// Main tick logic
void TaskQueue::handle_tick_event()
{
    std::vector<std::function<void()>> executionList;

    for (auto it = eventQueue.begin(); it != eventQueue.end();)
    {
        if (it->ticksToExecute == 0)
        {
            // Add the function to the execution list
            executionList.push_back(it->anon);

            if (it->repeat)
            {
                // Reset ticksToExecute for repeating tasks
                it->ticksToExecute = it->initialTicks;
                ++it;
            }
            else
            {
                // Remove non-repeating tasks
                it = eventQueue.erase(it);
            }
        }
        else
        {
            // Decrement ticksToExecute
            --(it->ticksToExecute);
            ++it;
        }
    }

    // Execute all functions in the execution list
    for (auto &func : executionList)
    {
        func();
    }
}

// Clears the event queue
void TaskQueue::clean()
{
    eventQueue.clear();
}