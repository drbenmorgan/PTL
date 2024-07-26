#include "PTL/AutoLock.hh"
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
    void operator()() const
    {
        PTL::AutoLock l(PTL::TypeMutex<decltype(std::cout)>());
        std::cout << "Functor running on " << PTL::GetThreadId() << std::endl;
    }
};

int
main()
{
    // Create a threadpool with default config (so threads = number of cores)
    PTL::ThreadPool pool{ PTL::ThreadPool::Config{} };

    // Can execute given function on each thread. Realistically only one returning
    // void as otherwise return goes into the ether...
    // NB: Is synchronous, so will only return after all executions are joined.
    pool.execute_on_all_threads(say_hello);

    // Can also pass functors.
    // NB: The same object instance is executed on all threads, but note that the
    // interface is pass-by-value. That single instance is therefore at best a move
    // of what's passed in, at worst a copy-move.
    pool.execute_on_all_threads(SaySomething{});

    // It also supports running lambdas
    pool.execute_on_all_threads([]() {
        PTL::AutoLock l(PTL::TypeMutex<decltype(std::cout)>());
        std::cout << "Lamdba running on thread " << PTL::GetThreadId() << std::endl;
    });

    // At this low level in PTL, this function must be called before pool's destructor is
    // called (Feels like a bug, or need for a higher level interface for pool creation)
    pool.destroy_threadpool();
}
