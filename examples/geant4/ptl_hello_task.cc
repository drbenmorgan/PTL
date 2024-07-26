#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
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
    // TaskGroup is next level above ThreadPool
    // This uses the internal threadpool by default, but we can pass one 
    // as a constructor argument if needed (see later)
    // It also takes a template argument to indicate the return type of all
    // calls that will be run in this group
    PTL::TaskGroup<void> group;

    // This is results in just one Task, but it is asynchronous (doesn't block
    // until `say_hello` completes)...
    group.exec(say_hello);
    // ... so it should be paired somewhere with a join     
    // Not doing so usually leads to a segfault (e.g. threads still running at program
    // exit), or side effects (tasks/results remain in the underlying queue until join())
    group.join();

    // Unlike ThreadPool case, can submit variable number of executions (here, Tasks)
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // ... and can attach/pass data to each Task, even adding more than number of
    // threads 
    for(const auto& item : data)
    {
        group.exec(say_hello);
        // Note: as before, we are passing by value here so do get copying!
        // Even so, for future reference when we resolve this, be careful, as:
        // - `SaySomething::operator()...` takes argument by reference
        // - Its execution must therefore be within the lifetime
        //   of the referee.
        // - Fine here, because lifetime of `data` is main(), execution of
        //   all `count` tasks happens in join right before end of main.
        group.exec(SaySomething{}, item);
    }

    // Though we must always finally join(), we can also use wait() as an intermediate 
    // synchronization point that ensures all tasks submited have completed...
    group.wait();

    // ... before join()ing somewhere else
    group.join();
}
