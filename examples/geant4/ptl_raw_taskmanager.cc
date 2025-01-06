#include "PTL/AutoLock.hh"
#include "PTL/TaskManager.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>

// Free Function
void
say_hello()
{
    // Purely so we don't mix outputs (comment out and see what happens...)!
    PTL::AutoLock l(PTL::TypeMutex<decltype(std::cout)>());
    std::cout << "Hello from thread " << PTL::GetThreadId() << std::endl;
}

// Simple Functor
struct SaySomething
{
    void operator()(const std::string& data) const
    {
        PTL::AutoLock l(PTL::TypeMutex<decltype(std::cout)>());
        std::cout << "Functor " << this << " running with data '" << data
                  << "' on thread " << PTL::GetThreadId() << std::endl;
    }
};

int
main()
{
    // TaskManager is next level above ThreadPool
    // This uses the internal threadpool by default, but we can pass one
    // as a constructor argument if needed (see later)
    // Create a threadpool with default config (so threads = number of cores)
    auto pool = new PTL::ThreadPool{ PTL::ThreadPool::Config{} };
    auto tm   = std::make_unique<PTL::TaskManager>(pool);

    // This is results in just one Task, but it is asynchronous (doesn't block
    // until `say_hello` completes)...
    auto task = tm->async(say_hello);
    // ... so it should be paired somewhere with a join
    // Not doing so usually leads to a segfault (e.g. threads still running at program
    // exit), or side effects (tasks/results remain in the underlying queue until join())
    task->wait();

    // Unlike ThreadPool case, can submit variable number of executions (here, Tasks)
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // ... and can attach/pass data to each Task, even adding more than number of
    // threads
    std::vector<std::shared_ptr<PTL::TaskFuture<void>>> tasks;
    for(const auto& item : data)
    {
        tasks.emplace_back(tm->async(say_hello));
        tasks.emplace_back(tm->async(SaySomething{}, item));
    }

    // Wait on all the tasks
    for(const auto& task : tasks)
    {
        task->wait();
    }
}
