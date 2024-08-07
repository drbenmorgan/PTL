# Examples for Geant4 Initialization in Parallel Task

The examples here are intended as a simple step-by-step buildup through
the capabilities on PTL useful withing scope of Geant4's workitem on
parallelizing its initialization stage. They are deliberately minimal
and less convoluted than PTLs examples, additionally showing some gotchas
in the interfaces/behaviour.

Short summaries are listed below, with detailed technical parts covered by
comments in the source code.

## `ptl_hello.cc`
Demonstrates basic use of PTL's low level `ThreadPool` class. The primary
case shown is how the same thing (free function, Functor, or lambda) can be executed 
on all threads. Key points:

- Only for operations that must be executed on every single thread in the pool, such as
  those that rely on `thread_local` variables.
- Operations are synchronous, or rather, `execute_on_all_threads` blocks until
  all threads have completed their work.
- No direct way to pass different data to each thread, or get results back (and 
  see later on copy/move semantics).

## `ptl_hello_task.cc`
Demonstrates basic use of PTL's `TaskGroup` class to do the same operations
we did in `ptl_hello.cc`. Key points:

- Asynchronous submission of work, Tasks, that may or may not run on any given thread in the pool.
- Tasks are not guaranteed to run on a specific thread.
- Tasks as free functions, lambdas, or Functors.
- Submission of different data with each Task.
- Synchronization in thread where Tasks submitted from.

## `ptl_copy_move.cc`
Simple but important demonstration of the copy-move semantics of passing
arguments to `TaskGroup::exec` and similar (`ThreadPool::execute_on_all_threads`).
Key points:

- Default is pass-by-value, so Tasks get copy/move of the data they will work on.
- Need to use ref/cref bindings or lambda captures if explicit pass-by-reference is wanted.
- Need to be aware of possible differences in data/execution lifetimes to avoid
  issues like dangling pointers.

## `ptl_exec_join.cc`
Builds on previous examples to show how `TaskGroup::join` can be used to collect
non-void return values from submitted Tasks. Key points:

- How to specify non-void TaskGroup and join function.
- Simple N->1 join operation.
- That join operation is sequential over the Task results in the TaskGroup's thread.

## `ptl_exec_join_vector.cc`
The same as `ptl_exec_join.cc` but demonstrating that the join operation
can have a different type from the result type of the Tasks. Key points:

- Join function for differing result/join types.
- Simple N->N join operation.

## `ptl_vector_to_vector.cc`
A slightly different way to organise the computation in `ptl_exec_join_vector.cc`.
We realize that we are doing a N->N mapping with each element independent. So
instead of "joining" results into a final vector, can also pre-allocate result
vector, then each task computes and sets each element. Thread-safe here as each
task only ever works on one memory location. Key points:

- May be different ways to organise input/Task/output.
- Thread safety can be maintained, but care needed. 

## `ptl_vector_subtask.cc`
Another take on `ptl_vector_to_vector.cc` this time showing that the function
called in a task can itself launch other tasks to complete a subset of work.
Only simple, totally independent tasks are considered here. Recursive tasking,
i.e. a function that launches Tasks of itself, need care and are considered
separately. Key points:

- Tasks can launch other tasks, the global threadpool being used by default.
- Blocks of work can be sent to tasks.

## `ptl_exec_member_function.cc`
Demonstrates that we can also call member functions on existing object instances,
the easiest way being through lambdas. Though not shown for clarity, how to pass
data to the member function calls can also be handled through lambda captures and
the techniques shown in previous exercises. It is left as an exercise if you are
interested. Key points:

- Wrapping of member function calls on objects through lambdas.
- Can call more than one member function in Task if required, or easiest for
  efficient computation.

## Handling `thread_local` data and Tasks that insert data in a global store
Tasking works best with computations that don't rely on `thread_local` data
as there is no guarantee of tasks running on all threads (see use of
`ThreadPool::execute_on_all_threads` in `ptl_hello.cc` if that is required).

Similarly, tasks that may insert data in a global store, or otherwise perform
non-threadsafe operations on shared data require careful use of locking or similar.
These are left as a semi-obvious exercise, or to be shown as use-cases in Geant4
are identified


## Use of Ranges (TODO/Optional)
Ranges are a C++20/23 [standard library](https://en.cppreference.com/w/cpp/ranges), also
available for earlier standards in the [`ranges-v3` library](https://github.com/ericniebler/range-v3)
which the C++ Standard adopted. These are usable in Geant4 given its use of C++17, but
use here would require import on `ranges-v3` as an external.

TODO: May illustrate them here despite this limitation as they are extremely useful
for implementing Task-related operations.

### Enumerations
For when we want to use range-for, but know how far we are from the start, e.g.

```c++
#include <iostream>
#include <vector>

#include <range/v3/view/enumerate.hpp>

int main()
{
    constexpr static auto v = {'A', 'B', 'C', 'D'};

    for (auto const [index, letter] : ranges::views::enumerate(v))
        std::cout << index << ':' << letter << std::endl;

    // Outputs 0:A 1:B etc...
}
```

### Zips
For when we want to iterate over two or more ranges together in sync, e.g.

```c++
#include <iostream>
#include <vector>

#include <range/v3/view/enumerate.hpp>
 
int main()
{
    constexpr static auto latin = {'a', 'b', 'c', 'd'};
    constexpr static auto greek = {"α", "β", "γ", "δ"};
    for (auto const& [l, g] : ranges::views::zip(latin, greek))
        std::cout << l << ':' << g << std::endl;

    // Outputs a:α etc... 
}
```

### Chunks
For when we want to split range up into blocks to work on, e.g.

```c++
#include <algorithm>
#include <iostream>

#include <range/v3/view/chunk.hpp>
 
int main()
{
    const auto v = {1, 2, 3, 4, 5, 6, 7};
 
    for (const auto& chunk : ranges::views::chunk(v, 2))
    {
      std::cout << "[ ";
      for (const auto i : chunk)
        std::cout << i << ", ";
      std::cout << "]" << std::endl;
    }

    // outputs [ 1, 2, ] [3, 4, ] etc
}
```
