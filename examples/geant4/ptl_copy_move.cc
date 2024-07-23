#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>
#include <vector>

class ExpensiveToCopy
{
public:
    ExpensiveToCopy() { log("constructor"); }
    ~ExpensiveToCopy() { log("destructor"); }
    ExpensiveToCopy(const ExpensiveToCopy&) { log("copy-constructor"); }
    ExpensiveToCopy(ExpensiveToCopy&&) { log("move-constructor"); }
    ExpensiveToCopy& operator=(const ExpensiveToCopy&)
    {
        log("copy-assignment");
        return *this;
    }
    ExpensiveToCopy& operator=(ExpensiveToCopy&&)
    {
        log("move-assignment");
        return *this;
    }

    void log(std::string_view msg) const
    {
        std::cout << this << ": " << msg << std::endl;
    }
};

void
pass_by_constref(const ExpensiveToCopy& val)
{
    val.log("pass_by_constref called!");
    return;
}

void
pass_by_pointer(const ExpensiveToCopy* val)
{
    val->log("pass_by_pointer called!");
    return;
}

int
main()
{
    // "Classic" case
    // Simple input value, passing to const-ref function gives
    // - one construction
    // - one destruction
    // in this scope block, just as you'd expect
    std::cout << "Start non-tasking demo: " << std::endl;
    {
        ExpensiveToCopy x;
        pass_by_constref(x);
    }
    std::cout << std::endl;

    // "Seemingly obvious but isn't pass-by-ref" case
    // Based on what we've seen, can "exec" function with argument via a TaskGroup
    // Supplying value through exec *isn't* pass-by-ref in exec, but pass by value!
    // There are at least 4 temporaries created before the actual call
    // - Possibly a design issue in PTL, see https://github.com/jrmadsen/PTL/issues/49
    // Note:
    // - Absolutely fine for trivial to copy types like basic types or pointers
    // - Might be what you want in certain cases (but consider use of std::move etc)
    // - TODO: Not considering thread-local variables at this point!
    std::cout << "Start TaskGroup::exec uses pass-by-value demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        PTL::TaskGroup<void> group;
        group.exec(pass_by_constref, y);  // "wrong" in that it passes y by value to exec.
        group.join();

        // Use of auto and a range-for loop doesn't help either:
        std::cout << "- Start TaskGroup::exec uses pass-by-value demo (vector): "
                  << std::endl;

        std::cout << "- Constructing vector..." << std::endl;
        std::vector<ExpensiveToCopy> v(2);
        std::cout << "- Starting loop" << std::endl;
        for(const auto& e : v)
        {
            std::cout << "- iteration pass..." << std::endl;
            group.exec(pass_by_constref, e);  // This is also pass by value
        }
        group.join();
    }
    std::cout << std::endl;

    // Workarounds: Basically getting template argument deduction to work.
    // 1. Exec via a wrapping lamdba with capture by reference:
    std::cout << "Start TaskGroup::exec with lambda capture demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        PTL::TaskGroup<void> group;
        group.exec([&y]() { pass_by_constref(y); });
        group.join();
    }
    std::cout << std::endl;

    // 2. Clarify argument as ref using C++ tool for the job:
    // SA: https://en.cppreference.com/w/cpp/utility/functional/ref
    std::cout << "Start TaskGroup::exec with std::cref demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        PTL::TaskGroup<void> group;
        group.exec(pass_by_constref, std::cref(y));
        group.join();
    }
    std::cout << std::endl;

    // Passing pointers directly works as they're trivially copyable
    std::cout << "Start TaskGroup::exec with pointer demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        auto                 ptr = &y;
        PTL::TaskGroup<void> group;
        group.exec(pass_by_pointer, ptr);
        group.join();
    }
}
