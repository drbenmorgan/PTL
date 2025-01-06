#include "PTL/AutoLock.hh"
#include "PTL/Task.hh"
#include "PTL/ThreadPool.hh"
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
        std::cout << "Functor " << this << " running with data '" << data << "' on thread "
                  << PTL::GetThreadId() << std::endl;
    }
};

int
main()
{
    // Create a threadpool with default config (so threads = number of cores)
    PTL::ThreadPool pool{ PTL::ThreadPool::Config{} };

    // Add a task to the pool, note that here we use a shared pointer to the task,
    // and must specify the type and arguments of the task (here, void and none)
    auto task1 = std::make_shared<PTL::PackagedTask<void>>(say_hello);
    pool.add_task(task1);

    // Though a void, we can still get a future and wait on it, or wait/get directly
    task1->wait();

    // Can submit variable number of executions (here, Tasks)
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // ... and can attach/pass data to each Task, even adding more than number of
    // threads. As above, we store the tasks in a vector so we can wait on them.
    std::vector<std::shared_ptr<PTL::TaskFuture<void>>> tasks;

    for(const auto& item : data)
    {
        auto hello_task = std::make_shared<PTL::PackagedTask<void>>(say_hello);
        pool.add_task(hello_task);
        tasks.emplace_back(hello_task);
         
        auto say_task = std::make_shared<PTL::PackagedTask<void, const std::string&>>(SaySomething{}, std::cref(item));
        pool.add_task(say_task);
        tasks.push_back(say_task);
    }

    // Wait on all the tasks
    for(const auto& task : tasks)
        task->wait();

    // At this low level in PTL, this function must be called before pool's destructor is
    // called (Feels like a bug, or need for a higher level interface for pool creation)
    pool.destroy_threadpool();
}
