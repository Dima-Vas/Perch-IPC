# Crossplatform C++ IPC library 

This library provides a simple C++ interprocess communication interface for multiple platforms including Windows and POSIX-compatible operating systems like Linux and FreeBSD.

## Content
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Documentation](#documentation)

## Prerequisites
To install the library, you will only need some build instruments : 
- <code>make</code> v4.3+
- <code>cmake</code> v3.5+

## Installation
1) In the cloned repository root directory, run :

<code>cmake -S . -B ./cmake/build -D[WIN32 | UNIX]=</code>

Use either <code>-DWIN32=</code> or <code>-DUNIX=</code> defined depending on the build you need. If you do cross-compilation, you may also be willing to specify the sysroot folder via

<code>-DCMAKE_INSTALL_INCLUDEDIR=/sysrootpath/usr/local/inc</code>

and 

<code>-DCMAKE_INSTALL_LIBDIR=/sysrootpath/usr/local/lib</code>

2) Run
<code>cd cmake/build && make && make install</code>

3) Voila! Include some <PIPC/api/***> headers and you are good to go.

## Usage
Everything you can utilize in this library revolves around classes declared in PIPC/api headers. They offer you the following functions : 

- <code>Process</code> class - an entity for the processes you initialize yourself in your code.
- <code>Pipe</code> class - an entity representing pipes between Process entities for communication.
- <code>ProcessCreation</code> - a functional class that provides the API to rely on while developing: starting, waiting for, and killing processes. organizing IPC via pipes.
- <code>SharedMutex</code> - a simple process-level implementation of mutexes. Unlike Process and Pipe entities, SharedMutex ones can be used as separate interprocess synchronization instruments.
- <code>SharedMemory</code> - abstracts the interface of shared memory mechanisms and provides data-race-safe operations for utilizing it : reading, writing, compare-and-swap check and write blocking.

Find some examples of real-life usage of them all in the documentation

## Documentation
