# Distributed Systems Coursework

Course work for BSE3202

## A Sequencer-based multicast protocol using Java RMI (Project 1)

Your task is to create a sequencer-based multicasting service, using a slight variant of the simple version
of the Amoeba protocol (the first protocol described [here](www.cdk3.net/coordination)), and a small client
application to exercise it. You will use Java RMI and multicast sockets.

- The main classes in your implementation will be:

  - **TestSequencer.java** – Each instance (you will create several) will allow the user to enter
    strings and multicast them to a group of instances of TestSequencer. TestSequencer will
    also be capable of “stress-testing” – multicasting messages as fast as possible.

  - **Group.java** – TestSequencer uses an instance of Group for group communication
    services. Group in turn uses a MulticastSocket to receive incoming messages, and uses RMI
    to the sequencer for sending messages and other, sequencer-specific operations

  - Sequencer.java, SequencerImpl.java – the interface to and implementation of a sequencer.

  - **History.java** – the implementation of the sequencer's history. History does not need to
    have a fixed capacity, but obsolete items should be removed whenever its size grows beyond a
    threshold, say 1024 entries.

### Note

- Attached you will find:

  - The RMI interface to an object which will implement the sequencer(Sequencer.java),

  - Two trivial classes associated with the Sequencer definition(SequencerJoinInfo.java and SequencerException.java)

  - Outline code for Group.java, and

  - Some hints on marchalling messages into datagram packets.

- You need to write SequencerImpl.java and History.java from scratch.

- The scheme you produce will not precisely implement the Amoeba protocol. In that protocol,
  messages are sent unreliably to the sequencer in the first instance. In yours, messages to be multicast are
  handed to the sequencer using RMI.

### Choosing a multicast address and time-to-live

SequencerImpl should take as an argument the IP multicast address that the corresponding Groups are to
use. To avoid collisions with numbers chosen by others, incorporate a random number into the multicast
IP address that you use – e.g. 234.day.month.rand where rand is a random number between 1 and 254.

Don't set your sockets' time-to-live (TTL) to more than 1. That way, your multicast packets will not be
transmitted beyond the local router.

## Using RPC in C (or C++) to implement a Task Bag(Project 2)

In this exercise we shall implement the Task Bag as a Remote “Object” (using RPC in C or C++) and to use it as a basis for
performing a parallel computation on several workstations.

Three roles are involved: The Task Bag, the master process, and the worker processes. The master and
worker processes are clients of the Task Bag “object”.

### The Task Bag

The functionality of the Task Bag is to provide a repository for Pairs. Each Pair may be regarded as a task
description. A Pair consists of two parts – a key and a value. The value contains the actual description ofa task and the key is anything that can be used to reference the Pair. A typical key might be a task name
or number. A task description may be used by the master to describe tasks and by workers to describe
results. Clients may: i) add task descriptions to the Task Bag, ii) remove them from the Task Bag iii) retrieve
them from the Task Bag. To access a task description, the client must specify a key.

The Task Bag will offer the operations pairOut, pairIn and readPair in its “interface”. They are
defined as follows:

| Function               | Definition                                                                                                                                                                                                                                                                                                                    |
| ---------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| pairOut(key, value)    | Causes a Pair (key, value) to be added to the Task Bag. The client process continues immediately                                                                                                                                                                                                                              |
| pairIn(key) -> value   | Causes some Pair in the Task Bag that matches key to be withdrawn from the Task Bag; the value part of the Pair is returned and the client process continues. If no matching Pair is available, the client waits until one is and then proceeds as before. If several matching Pairs are available, one is chosen arbitrarily |
| readPair(key) -> value | Is the same as pairIn(key) except that the Pair remains in the Task Bag                                                                                                                                                                                                                                                       |

### The application

You should choose one application that requires fairly intensive computation to carry it out and which is
easily divided into a number of identical subtasks. You could consider tasks such as:

- Parallel compilation of the modules of a program (parallel make)

- Searching for files containing a particular text string

- Finding prime numbers

- Matrix multiplication or

- Fractal images

### Parallel Programming with the Task Bag

We consider the sort of parallel program that involves a transformation or series of transformations to be
applied to all the elements of some set in parallel. This type of parallelism is suitable for modelling with
the master-worker paradigm.

A master process provides a set of tasks to be done by a collection of identical worker processes. Each
worker is capable of performing any one of the steps in a particular computation. In the simplest cases,
there is only one step. A worker repeatedly gets a task, carries it out and then puts the result in the Task
Bag. The results are collected by the master. The program executes in the same way whether there are 1,
10 or 1000 workers.

We refer to two examples throughout this explanation. In the first example, the joint task is to generate
all the prime numbers less than some limit, MAX. We use one master process together with one or more
worker processes. The master process sets up the first task and then waits to collect the prime numbers
calculated by the workers. Each worker process repeatedly gets a range of numbers within which to search
for prime numbers. Each worker places the sets of primes it finds in the Task Bag, from whence the master
may collect them.

For the second example, we consider a program that multiplies two matrices A and B. In this program one
master process sets up the multiplication tasks and collects the results, generated by one or more
workers. Each worker repeatedly gets an element to calculate and puts the result in the Task Bag (for later
collection by the master).

### How the workers know which task to do next

In many computations, there is a collection of tasks, numbered First to Last. Each worker repeatedly
carries out one (or a group) of the tasks. Before a worker starts it needs to know which task to do next. A
Pair with the key Next Task can be used for this purpose. The master puts in the first task and each worker
in turn takes the Pair out, increments its value and puts it back.

The number of tasks done together is a per application constant (GRANULARITY). When there are no more
tasks to be done, the worker does not replace Next Task in the Task Bag. When other workers attempt to
remove it, they will have to wait. No more work will be done until the master supplies another collection
of tasks to calculate.

In the prime numbers example, the worker calculates primes within the range nextElement to
nextElement + GRANULARITY-1. In the matrix multiplication example, GRANULARITY = 1 and the worker
calculates the row and column of the element to calculate from the value retrieved. e.g. the elements
may be numbered in order across the rows.

### The workers' results

It is important to note that many workers perform similar tasks and generally return values with identical
keys to the Task Bag. The Task Bag must be implemented so that many Pairs with the same key may be
held at the same time. (That is, it is not a set!)

In the prime numbers example, all the results calculated by the workers may bear the same key: Result.
A worker can put a collection of prime numbers in the Task Bag as follows:

```c
pairOut(“Primes”, <a collection of primes>);
```

The master just collects all the Pairs with the key Primes e.g., by:

```c
<a collection of primes> = pairIn(“Primes”)
```

In some applications, each worker needs to apply a different key to the results of its work. For example,
in the matrix multiplication, each worker task consists of calculating one element of the result: the key of
the result needs to indicate the row and column numbers of the element calculated. When a worker has
calculated an element (row, column), it will specify the number of the row and column in the result e.g.:

```c
sprintf(key, “Element%d%d”, row, column);
pairOut(key,<the calculated result>);
```

### Data for the workers

In some computations, the workers need data in order to perform their task. For example, in the matrix
multiplication task, a worker needs row i of matrix A and column j of matrix B in order to multiply them
together. This data is put in the Task Bag by the master and may be accessed by workers that need it. The
master can put in the rows of matrix A and the columns of matrix B as follows:

```c
pairOut("A1", <A's first row>);
pairOut("A2", <A's second row>);
// ...
pairOut("B1", <B's first column>);pairOut("B2", <B's second column>);
// ...
```

In this example, many workers will require the same rows and columns, they therefore use the
`readPair` operation rather than the `pairIn` operation. A worker may for example access a particular
row of A and column of B as follows (in C!):

```c
sprint(key, “A%d”, row);
aRow = readPair(key);
sprintf(key, “B%d”, column);
bRow = readPair(key);
```

In the calculation of prime numbers, a worker calculates whether a number, n is prime by dividing it by all
the prime numbers up to √n. Therefore the worker needs to know the previously calculated primes up to
√n. As the master collects the primes calculated by the worker

### Monitoring

The above arrangement is not fault-tolerant. If a worker fails before completing a task, the master will
have to wait when it attempts to read the corresponding result. In our example, if a worker fails between
removing the value of Next and replacing the next value, all the workers will have to wait for an indefinite
time.

The user who starts the parallel computation should be able to monitor its progress. The monitor should
report on the state of the computation and provide the ability to recover from incomplete computations.
This may require you to add some operations in the “interface” of the Task Bag.

### Synchronization of client operations

There are several approaches to the case where no matching pair is available for a client performing an
`pairIn` or a `readPair`.

We suggest that you implement the Task Bag first so that the client ‘polls’ the Task Bag, repeating the
request after a small time out, if the Task Bag tells it no pair is currently available. This is not an ideal
approach, but you may describe a better approach and if you have time, implement it.
