#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>
#include <vector>

// Trivial character count task
size_t
count(const std::string& word)
{
    return word.size();
}

int
main()
{
    // Data as before, here we're going to calculate the number of characters in each word
    std::vector<std::string> data = {
        "foo",  "bar",   "baz",    "FOO",  "BAR",   "BAZ",
        "quux", "corge", "garply", "QUUX", "CORGE", "GARPLY"
    };

    // TaskGroup's first template parameter is the type of "joining" the results from
    // each task. Void returns don't need anything, but if we want to return something,
    // then we need to specify a function to do the joining.
    // This time, our result is going to be a vector of character counts, with each call
    // to the join pushing back the result value
    auto join_func = [](std::vector<size_t>& output_ref, size_t task_result) {
        output_ref.push_back(task_result);
        return output_ref;
    };
    // Because the group result type (vector) and task result type (size_t) are different,
    // must specify them both
    PTL::TaskGroup<std::vector<size_t>, size_t> group(join_func);

    // Submitting data is same as before..
    for(const auto& item : data)
    {
        group.exec(count, item);
    }

    // ... as before **MUST** join at some point to ensure all Tasks have completed
    // Result type this time is vector, and it should be consistent across different runs
    // and/or number of threads as the tasks should be queued/popped in order.
    auto result = group.join(/*optional initial value*/);
    for(auto val : result)
    {
      std::cout << "Chars = " << val << std::endl;
    }
}
