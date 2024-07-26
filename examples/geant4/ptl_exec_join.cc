#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>

// Trivial character count task
size_t
count(const std::string& word)
{
    return word.size();
}

int
main()
{
    // Data as in `ptl_exec_join.cc`, here we're going to calculate the total number 
    // of characters
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // TaskGroup's first template argument is the type of the result we want to get
    // by "joining" the return values from each task. If our tasks are going to return 
    // anything other than `void` and we want to get these return values, we have to
    // construct the TaskGroup with:
    // 1. A template argument indicating the return type of the "join" operation
    // 2. A function/function-like object to take the result from each Task and "join"
    //    it into the final result. 
    //
    // For counting a total across Tasks, we just need a simple accumulator of the 
    // character counts from each task
    auto join_func = [](size_t& output_ref, size_t task_result) {
        output_ref += task_result;
        return output_ref;
    };
    PTL::TaskGroup<size_t> group(join_func);

    // Submitting data is same as before...
    for(const auto& item : data)
    {
        group.exec(count, std::cref(item));
    }

    // ... as before **MUST** join at some point to ensure all Tasks have completed
    // This time we get a return value from the join which is the result of applying the join 
    // function over the results from each task.
    // NB: Join is called SEQUENTIALLY over each result on the thread on which the TaskGroup was
    // created/execed.
    auto result = group.join(/*optional initial value*/);
    std::cout << "Total number of characters = " << result << std::endl;
}
