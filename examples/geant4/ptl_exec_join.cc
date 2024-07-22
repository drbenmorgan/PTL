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
    // Data as before, here we're going to calculate the total number of characters
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // TaskGroup's first template parameter is the type of "joining" the results from
    // each task. Void returns don't need anything, but if we want to return something,
    // then we need to specify a function to do the joining.
    // Here, we just need a simple accumulator of the character counts from each task
    auto join_func = [](size_t& output_ref, size_t task_result) {
        output_ref += task_result;
        return output_ref;
    };
    PTL::TaskGroup<size_t> group(join_func);

    // Submitting data is same as before..
    for(const auto& item : data)
    {
        group.exec(count, item);
    }

    // ... as before **MUST** join at some point to ensure all Tasks have completed
    // This time we get a return value which is the result of applying the join function
    // over the results from each task.
    auto result = group.join(/*optional initial value*/);
    std::cout << "Total number of characters = " << result << std::endl;
}
