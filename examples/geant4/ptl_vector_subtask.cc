#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>
#include <vector>

void
count_kernel(const std::string& in, size_t& out)
{
    out = in.size();
}

void
count(size_t begin, size_t end, const std::vector<std::string>& input,
      std::vector<size_t>& output)
{
    // Here we're actually in a thread, TaskGroup will pick up global ThreadPool
    PTL::TaskGroup<void> subgroup;
    for(size_t i = begin; i < end; ++i)
    {
        subgroup.exec(count_kernel, std::cref(input[i]), std::ref(output[i]));
    }
    // Join before we exit - we are asynchronous with outer task, but want to
    // ensure all of *our* tasks complete before returning (and thus allowing
    // outer task to join())
    subgroup.join();
}

int
main()
{
    // Data as before, again we're going to calculate the number of characters in each
    // word, but break the work up into 3 top level tasks, each of which will launch
    // 4 subtasks. In other words, one main task per row in `input`, one subtask per
    // element.
    std::vector<std::string> input = { "foo",    "bar",  "baz",   "FOO",
                                       "BAR",    "BAZ",  "quux",  "corge",
                                       "garply", "QUUX", "CORGE", "GARPLY" };

    // Here we know upfront that we're doing a N->N mapping, so we can preallocate
    // the output vector
    // NB: this is doing full allocation up front (not reserve()/push_back()).
    std::vector<size_t> result(input.size(), 0);

    // Only need a void TaskGroup, since all calculation is internal
    PTL::TaskGroup<void> group;

    // Loop creation of tasks is as before, but here use a "range" to submit
    // blocks of work as tasks, which will then decide how to organise that work.
    for(size_t i = 0; i < input.size(); i += 4)
    {
        group.exec(count, i, i + 4, std::cref(input), std::ref(result));
    }
    group.join();

    for(auto val : result)
    {
        std::cout << "Chars = " << val << std::endl;
    }
}
