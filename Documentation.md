## Documentation

### Process Class
The `Process` class represents a process entity within the codebase. It encapsulates essential information such as the executable path, arguments, and process ID. Key functionalities and attributes include:

- **Constructors**: Provides both default and parameterized constructors to initialize a processing entity.
- **Argument Management**: Allows appending additional arguments to the process via the `addArgument()` method.
- **Accessors**: Methods like `getPath()` and `getArguments()` enable retrieval of the executable path and arguments.
- **Process ID**: Provides access to the process ID through `setPID()` and `getPID()` methods.

### ProcessCreation Class
The `ProcessCreation` class serves as an interface, offering an API for basic manipulations of `Process` entities. Its functionalities include:

- **Process Operations**: Methods for launching (`launch()`), waiting for exit (`wait_for_exit()`), and terminating (`kill()`) processes.
- **Inter-Process Communication**: Provides a method (`pipe_IPC()`) to establish an inter-process communication mechanism using pipes between two processes.

### SharedMemory Class
The `SharedMemory` class manages and represents shared memory chunks. Its functionalities include:

- **Initialization**: Initializes shared memory with a unique name and a specified size using the constructor.
- **Memory Operations**:
  - `write()`: Writes a new element to the shared memory at a specified index or the first available position.
  - `read()`: Reads an element from the shared memory at a specified index.
  - `compare_and_swap()`: Performs a Compare-And-Swap operation, swapping the value at the provided index with a new value.
- **Memory Management**:
  - `freeze()`: Makes the memory unable to be written, preventing further modifications.
  - `is_frozen()`: Checks if the memory is frozen.
  - `get_size()`: Retrieves the size of the shared memory chunk.

### SharedMutex Class
The `SharedMutex` class implements an IPC mutex for synchronization among processes. Its functionalities include:

- **Initialization**: Initializes the mutex with a unique name for identification using the constructor.
- **Locking and Unlocking**:
  - `lock()`: Acquires the mutex, allowing exclusive access to shared resources.
  - `unlock()`: Releases the mutex, allowing other processes to access shared resources.

### Pipe Class
The `Pipe` class represents an entity for managing inter-process communication via named pipes. Its functionalities include:

- **Constructor**:
  - Default constructor and a constructor that accepts a pair of file descriptors for input and output.
- **Connect Processes** (`connect`): Connects two `Process` objects via a named pipe, establishing a communication link between them.
- **Transfer Data** (`transfer`): Initiates the transfer of data between the linked processes through the pipe.
- **Semaphores**:
  - Manages semaphores (POSIX or Windows based on the platform) to synchronize the launch of processes linked by the pipe.
