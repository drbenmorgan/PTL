#include "PTL/AutoLock.hh"
#include "PTL/TaskGroup.hh"
#include "PTL/Threading.hh"
#include "PTL/Types.hh"

#include <iostream>
#include <map>
#include <vector>

class SomeData
{
public:
    void process() { last_tid_ = PTL::GetThreadId(); }
    int  get_result() const { return last_tid_; }

private:
    int last_tid_;
};

int
main()
{
    // Some data to work on...
    std::vector<SomeData> input(100);

    // 
    PTL::TaskGroup<void> group;
    for(auto& item : input)
    {
        // Easiest way to exec a member function call on an object is
        // to wrap it in a lamdba with capture-by-ref. Any data needing
        // to be passed to the member function call could be captured in
        // the same manner. Remember that lambda capture handles pass-by-ref for us!
        group.exec([&item]() { item.process(); });
    }
    group.join();

    // -----
    // If more than one call is needed, can split up into separate
    // set of tasks:
    auto join_func = [](std::map<int, int>& output_ref, int task_result) {
        output_ref[task_result] += 1;
        return output_ref;
    };
    PTL::TaskGroup<std::map<int, int>, int> collate(join_func);

    for(const auto& item : input)
    {
        collate.exec([&item]() { return item.get_result(); });
    }
    auto collation = collate.join();

    std::cout << "Tasks used " << group.pool()->size()
              << " available threads as follows:" << std::endl;
    int total_tasks = 0;
    for(const auto [tid, count] : collation)
    {
        std::cout << "thread " << tid << " ran " << count << " tasks" << std::endl;
        total_tasks += count;
    }
    std::cout << "Total executed tasks = " << total_tasks << std::endl;

    // -----
    // ... Alternately, have the lambda organise the work. Comes down to what the
    // computation involves/relies on.
    for(auto& item : input)
    {
        collate.exec([&item]() {
            item.process();
            return item.get_result();
        });
    }
    collation = collate.join();

    std::cout << std::endl
              << "In-one execution used " << group.pool()->size()
              << " available threads as follows:" << std::endl;
    total_tasks = 0;
    for(const auto [tid, count] : collation)
    {
        std::cout << "thread " << tid << " ran " << count << " tasks" << std::endl;
        total_tasks += count;
    }
    std::cout << "Total executed tasks = " << total_tasks << std::endl;
}
