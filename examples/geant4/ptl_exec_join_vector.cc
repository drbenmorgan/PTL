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

    // TaskGroup join functions can have different types for the join operation and
    // the task results, so we have lots of options here.
    // This time, the result of our join is going to be a vector, with each element being
    // the character count.
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
        group.exec(count, std::cref(item));
    }

    // ... as before **MUST** join at some point to ensure all Tasks have completed
    // Result type this time is vector, and it should be consistent across different runs
    // and/or number of threads as the tasks should be queued/popped in order they were
    // queued. Will see later how better to ensure this if we need to guarantee ordering.
    //
    // Unlike submission of data, getting the return value is usually o.k.,
    // move/copy-elision under the hood minimizing copies. However, make sure types have
    // appropriate copy/move constructors and use appropriate value/lvalue/rvalue refs in
    // then join function. Tasks returning expensive to move types should probably return
    // these wrapped in a unique_ptr or similar. We'll see other ways to handle data
    // to/from tasks in later examples.
    auto result = group.join(/*optional initial value*/);
    for(auto val : result)
    {
        std::cout << "Chars = " << val << std::endl;
    }
}
