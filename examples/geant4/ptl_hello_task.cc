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
        std::cout << "Functor running with data '" << data << "' on thread "
                  << PTL::GetThreadId() << std::endl;
    }
};

int
main()
{
    // TaskGroup? What is next level above ThreadPool
    // This uses the internal threadpool by default, but we can pass one if needed
    PTL::TaskGroup<void> group;

    // This is just one Task, but it is asynchronous
    group.exec(say_hello);
    // ... so it should be paired somewhere with a join
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
        group.exec(SaySomething{}, item);
    }
    // ... as before **MUST** join at some point to ensure all Tasks have completed
    // Not doing so usually leads to a segfault.
    group.join();
}
