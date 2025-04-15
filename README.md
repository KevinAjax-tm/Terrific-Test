This project is a **multi-threaded task scheduler and process monitor** implemented in C, designed to manage and execute tasks across multiple threads while providing real-time system monitoring. Here's a detailed breakdown:

---

### **Core Components**
1. **Thread Pool Manager**  
   - Creates/manages worker threads (configurable count)  
   - Assigns tasks from a queue to available threads  
   - Implements thread-safe task distribution

2. **Task Queue**  
   - Circular buffer for task storage  
   - Mutex-protected push/pop operations  
   - Condition variables for thread synchronization

3. **Subprocess Execution**  
   - Uses `fork()` + `execvp()` to run tasks in isolated processes  
   - Implements timeouts to prevent hanging tasks  
   - Tracks process exit codes

4. **Shared Memory IPC**  
   - POSIX shared memory (`shm_open`) for inter-process communication  
   - Stores task results with mutex protection  
   - Allows real-time monitoring of system state

5. **Process Supervisor**  
   - `SIGCHLD` handler to track process termination  
   - Detects crashes/unexpected exits  
   - Maintains active process registry

6. **ncurses-based UI**  
   - Real-time dashboard showing:  
     - Thread status (BUSY/IDLE)  
     - Active tasks  
     - System resource usage  

---

### **Key Features**
- **Concurrent Task Execution**: Parallel processing via thread pool  
- **Fault Tolerance**: Process monitoring and crash detection  
- **Resource Management**: Thread recycling, memory cleanup  
- **Real-time Feedback**: Terminal UI with live updates  
- **POSIX Compliance**: Portable across Unix-like systems  

---

### **Workflow**
1. **Task Submission** → Added to queue  
2. **Thread Pool** → Workers fetch tasks  
3. **Subprocess** → Each task runs in isolated process  
4. **Supervisor** → Monitors process health  
5. **IPC** → Results stored in shared memory  
6. **UI** → Displays live system state  

---

### **Technical Stack**
| Component       | Technologies Used               |
|-----------------|---------------------------------|
| Threading       | `pthread` (mutexes, condition variables) |
| Process Control | `fork()`, `execvp()`, `waitpid()` |
| IPC             | POSIX shared memory (`shm_*`)   |
| UI              | `ncurses` (terminal dashboard)  |
| Error Handling  | `errno`, `strerror()`, signal handlers |

---

### **Build & Run**
```bash
make clean && make  # Builds 'distask' executable
./distask [thread_count]  # Run with optional thread count (default=8)
```

---

### **Use Cases**
- Batch job processing  
- System monitoring tools  
- Parallel computation frameworks  
- Educational example for advanced C programming  

---

### **Project Structure**
```
distask/
├── include/       # Headers (.h)
├── src/           # Implementation (.c)
├── Makefile       # Build configuration
└── distask        # Final executable
```

The project demonstrates robust systems programming in C, combining threading, process management, IPC, and terminal UI concepts into a cohesive tool.
