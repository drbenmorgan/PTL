# Examples for Geant4 Initialization in Parallel Task

The examples here are intended as a simple step-by-step buildup through
the capabilities on PTL useful withing scope of Geant4's workitem on
parallelizing its initialization stage. They are deliberately minimal
and less convoluted that PTLs examples, additional showing some gotchas
in the interfaces/behaviour.

Short summaries are listed below, with detailed technical parts covered by
comments in the source code.

## `ptl_hello.cc`
Demonstrates basic use of PTL's low level `ThreadPool` class. The primary
case shown is how the same thing (free function, Functor, or lambda) can be executed 
on all threads. Key points:

- Only for operations that must be executed on every single thread in the pool.
- Operations are synchronous, or rather, `execute_on_all_threads` blocks until
  all threads have completed their work.
- No direct way to pass different data to each thread, or get results back (and 
  see later on copy/move semantics).

## `ptl_hello_task.cc`
Demonstrates basic use of PTL's `TaskGroup` class to do the same operations
we did in `ptl_hello.cc`. Key points:

- Asynchronous submission of work, Tasks, that may or may not run on any given thread in the pool.
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

## Vector of independent objects, call void method on each one
- Task per object, or blocked range
- Thread local limitations, if any

## Tasks which may be independent, but which insert data in a global store
- Mutex use

## Subtasking?
- Maybe each task will have a random number of calculations to do, launch
  subtasks if there are "enough"?

## Use of Ranges (Optional)
Ranges are a C++20/23 [standard library](https://en.cppreference.com/w/cpp/ranges), also
available for earlier standards in the [`ranges-v3` library](https://github.com/ericniebler/range-v3)
which the C++ Standard adopted. These are usable in Geant4 given its use of C++17, but
use here would require import on `ranges-v3` as an external.

We illustrate them here despite this limitation as they are extremely useful
for implementing Task-related operations.

### Enumerations

### Zips

### Chunks
