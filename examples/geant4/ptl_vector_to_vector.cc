#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>
#include <vector>

void
count(const std::string& in, size_t& out)
{
    out = in.size();
}

// "kernel" like version
void
count_k(size_t index, const std::vector<std::string>& in, std::vector<size_t>& out)
{
    // NB, nominally more complex as would need expects on
    // - in.size() == out.size()
    // - index < in.size()
    // and care needed that only access single element of out
    out[index] = in[index].size();
}

int
main()
{
    // Data as before, again we're going to calculate the number of characters in each
    // word, but showing a slightly different way to acheive the same thing
    std::vector<std::string> input = { "foo",    "bar",  "baz",   "FOO",
                                       "BAR",    "BAZ",  "quux",  "corge",
                                       "garply", "QUUX", "CORGE", "GARPLY" };

    // Here we know upfront that we're doing a N->N mapping, so we can preallocate
    // the output vector
    // NB: this is doing full allocation up front (not reserve()/push_back()).
    std::vector<size_t> result(input.size(), 0);

    // Only need a void TaskGroup, since all calculation is internal
    PTL::TaskGroup<void> group;

    // Loop creation of tasks is as before, but have to use a classic loop as
    // the vector index is an input to each task.
    for(size_t i = 0; i < input.size(); ++i)
    {
        group.exec(count, std::cref(input[i]), std::ref(result[i]));
        // or kernel-like group.exec(count_k, i, std::cref(input), std::ref(result));
    }
    group.join();

    for(auto val : result)
    {
        std::cout << "Chars = " << val << std::endl;
    }
}
