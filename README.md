<h1 align="center">Simple Operating System Simulation </h1>

<p align="center">
  <em>Simulation of a simplified Operating System featuring Scheduler, Synchronization, and Paging-based Memory Management.</em><br/>
  <small>HCMC University of Technology — Faculty of Computer Science & Engineering</small><br/>
  <small>Author: <strong>Hồng Phi Trường</strong></small>
</p>

<hr/>

<h2> Objective</h2>
<p>
This assignment simulates core mechanisms of a simple Operating System (OS), including process scheduling,
synchronization, and virtual-to-physical memory management. The goal is to help students understand
how an OS manages CPU and RAM through scheduling algorithms and paging-based memory mapping.
</p>

<hr/>

<h2> Core Components</h2>

<h3> Scheduler (Multi-Level Queue)</h3>
<ul>
  <li>Implements <strong>Multi-Level Queue (MLQ)</strong> scheduling policy similar to Linux kernel.</li>
  <li>Each ready queue represents a specific priority level.</li>
  <li>Processes are picked up in round-robin manner within each priority level.</li>
  <li>Supports preemption and time-slicing using virtual timer interrupts.</li>
</ul>

<h3> Memory Management (Paging-based Virtual Memory)</h3>
<ul>
  <li>Implements <strong>paging memory system</strong> with per-process virtual address space.</li>
  <li>Supports both <strong>data segment</strong> (static allocation) and <strong>heap segment</strong> (dynamic allocation).</li>
  <li>Uses <code>mm-vm.c</code>, <code>mm-memphy.c</code>, and <code>mm.c</code> modules to simulate page mapping.</li>
  <li>Supports <strong>Heap Go-Down Strategy</strong> for efficient dynamic memory allocation.</li>
  <li>Includes basic operations: <code>ALLOC</code>, <code>MALLOC</code>, <code>FREE</code>, <code>READ</code>, and <code>WRITE</code>.</li>
  <li>Implements swapping between RAM and SWAP devices for memory overcommitment.</li>
</ul>

<h3> Synchronization</h3>
<ul>
  <li>Ensures safe access to shared resources (e.g., queues, memory structures).</li>
  <li>Protects critical sections using locks to avoid race conditions in multiprocessor environments.</li>
</ul>

<hr/>


<h2> Project Structure</h2>

<pre>
📦 simple-os
├── include/
│   ├── common.h        # Global structs and definitions
│   ├── cpu.h           # Virtual CPU
│   ├── sched.h         # Scheduler module
│   ├── os-mm.h         # Memory management structs
│   └── os-cfg.h        # System configurations
├── src/
│   ├── os.c            # Entry point
│   ├── sched.c         # MLQ scheduling
│   ├── mm.c / mm-vm.c  # Paging-based memory management
│   ├── mm-memphy.c     # Physical memory simulation
│   └── queue.c         # Queue operations
├── input/              # Configuration & process files
├── output/             # Simulation results
└── Makefile            # Build automation
</pre>

<hr/>

<h2>Key Results</h2>
<ul>
  <li>Scheduler correctly distributes processes across multiple CPUs using MLQ.</li>
  <li>Paging engine successfully isolates process address spaces and handles page faults/swaps.</li>
  <li>Synchronization guarantees correct execution order under concurrent resource access.</li>
</ul>

<hr/>

<h2> Learning Outcomes</h2>
<ul>
  <li>Understand the working principles of CPU scheduling and process management.</li>
  <li>Gain insights into virtual memory translation and paging mechanisms.</li>
  <li>Apply synchronization techniques to avoid race conditions in shared memory systems.</li>
</ul>

<hr/>

<h2> References</h2>
<ul>
  <li>CO2018 Assignment Description — Faculty of CSE, HCMUT (2024)</li>
  <li>Linux Kernel Documentation — Process Scheduling & Paging Mechanisms</li>
  <li>Silberschatz, Galvin, & Gagne — <em>Operating System Concepts</em></li>
</ul>

<hr/>

<p align="center">
  <sub><em>“A miniature OS — built from scratch, for understanding the real one.”</em></sub><br/>
  <sub><em> <strong>Hồng Phi Trường</strong></em></sub>
</p>
