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

    // 2. Clarify argument as ref using C++ tool for the job. Basically, function/arg
    //    binding: 
    //    - https://en.cppreference.com/w/cpp/utility/functional/ref
    std::cout << "Start TaskGroup::exec with std::cref demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        PTL::TaskGroup<void> group;
        group.exec(pass_by_constref, std::cref(y));
        group.join();
    }
    std::cout << std::endl;

    // 3. Passing pointers directly works as they're trivially copyable
    //    Similarly any "pointer view" like objects provided what they view has
    //    a lifetime that extends until after the join.
    std::cout << "Start TaskGroup::exec with pointer demo: " << std::endl;
    {
        ExpensiveToCopy      y;
        auto                 ptr = &y;
        PTL::TaskGroup<void> group;
        group.exec(pass_by_pointer, ptr);
        group.join();
    }

    // 4. DANGER: Now that we see how to properly pass be reference, we
    // must be careful with object lifetimes:
    // - Execution of task is not synchronous
    // - Potential for it to be executed *after* the lifetime of the passed 
    //   ref/pointer has expired
    // - Basically, the lifetime of the Task must be within the lifetime of
    //   all data it uses.
    //
    // Say we do:
    //
    // auto p = new int(10);
    // a_task_group.exec(something, p);
    // delete p;
    //
    // This might appear to work, but the actual execution of something(p) might
    // happen after the delete, and so try to access a dangling pointer.
    //
    // Similarly for:
    //
    // {
    //   int p = 10;
    //   a_task_group.exec(something, p);
    // }
    // group.join();
    //
    // with stack variables going out of scope.
}
