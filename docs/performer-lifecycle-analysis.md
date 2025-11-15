# Performer Lifecycle and Threading Analysis

**Date:** 2024

## Questions Addressed

1. If Performers are destroyed before the RunLoop, is that OK?
2. What if a Performer's task schedules another Performer?
3. What thread context does the Performer's task run in?

---

## Thread Context: Where Tasks Execute

### Answer: Tasks Execute on the RunLoop's Thread

**Key Code Path:**

```cpp
// Performer.cpp:61 - Registration
m_runLoop->registerDescriptor(m_pipe[READ_PIPE_IDX], RunLoop::READABLE, 
    [this] { this->onSignaled(); });

// Performer.cpp:119-124 - When pipe becomes readable
void Performer::onSignaled() {
    m_signaled = false;
    uint8_t buf[1];
    (void)read(m_pipe[READ_PIPE_IDX], buf, sizeof(buf));
    fireItems();  // Called from RunLoop thread
}

// Performer.cpp:127-144 - Task execution
void Performer::fireItems() {
    while(true) {
        std::shared_ptr<Item> each;
        {
            std::unique_lock<std::mutex> locked(m_mutex);
            if(m_items.empty())
                break;
            each = m_items.front();
            m_items.pop();
        }
        if(not m_closed)
            each->fire();  // Executes m_task() on RunLoop thread
    }
}
```

**Execution Flow:**
1. `perform()` is called from any thread
2. Task is queued and pipe is written to
3. RunLoop detects pipe is readable (on RunLoop's thread)
4. `onSignaled()` is called (on RunLoop's thread)
5. `fireItems()` executes tasks (on RunLoop's thread)

**Exception:** If `wait=true` and called from RunLoop thread, task executes immediately:
```cpp
// Performer.cpp:90-95
if(wait and m_runLoop->isRunningInThisThread()) {
    fireItems();
    if(task)
        task();  // Immediate execution on RunLoop thread
    return;
}
```

**Conclusion:** Performer tasks **always execute on the RunLoop's thread**, not the thread that called `perform()`.

---

## Performer Destruction Before RunLoop

### Critical Bug: Tasks Don't Execute During Close

**Code Analysis:**

```cpp
// Performer.cpp:69-82
void Performer::close() {
    if(not m_closed) {
        m_closed = true;  // ⚠️ Set BEFORE fireItems()
        
        if(m_runLoop)
            m_runLoop->unregisterDescriptor(m_pipe[READ_PIPE_IDX]);
        m_runLoop = nullptr;
        ::close(m_pipe[READ_PIPE_IDX]);
        ::close(m_pipe[WRITE_PIPE_IDX]);
        
        fireItems();  // Called with m_closed = true
    }
}

// Performer.cpp:127-144
void Performer::fireItems() {
    while(true) {
        // ... dequeue item ...
        if(not m_closed)  // ⚠️ This is FALSE during close()!
            each->fire();
    }
}
```

**Problem:**
- When `close()` is called, `m_closed` is set to `true` **before** `fireItems()`
- `fireItems()` checks `if(not m_closed)` before executing tasks
- **Result:** Queued tasks are **never executed** during destruction!

**What Happens:**
1. Tasks queued before `close()` are dequeued but **not executed**
2. Tasks are silently dropped
3. No error is reported
4. If tasks were waiting for completion (`wait=true`), they wait forever

**Is It OK?**
- **No!** This is a bug that causes:
  - Silent task loss
  - Potential deadlocks (if `wait=true`)
  - Resource leaks (if tasks hold resources)

**Fix Needed:**
```cpp
void Performer::close() {
    if(not m_closed) {
        m_closed = true;
        
        // Fire items BEFORE unregistering/closing
        fireItems();  // Move this up
        
        if(m_runLoop)
            m_runLoop->unregisterDescriptor(m_pipe[READ_PIPE_IDX]);
        m_runLoop = nullptr;
        ::close(m_pipe[READ_PIPE_IDX]);
        ::close(m_pipe[WRITE_PIPE_IDX]);
    }
}
```

But wait - there's another issue: `fireItems()` checks `m_closed`, so we need to either:
1. Remove the check in `fireItems()` and handle it differently
2. Set `m_closed` after `fireItems()` (but then items might be queued during close)

**Better Fix:**
```cpp
void Performer::fireItems() {
    bool wasClosed = m_closed;  // Remember state
    
    while(true) {
        std::shared_ptr<Item> each;
        {
            std::unique_lock<std::mutex> locked(m_mutex);
            if(m_items.empty())
                break;
            each = m_items.front();
            m_items.pop();
        }
        
        // Execute if not closed, OR if we're in the process of closing
        // (to allow queued items to execute during close)
        if(not wasClosed)
            each->fire();
    }
}

void Performer::close() {
    if(not m_closed) {
        m_closed = true;
        fireItems();  // Now items will execute
        // ... rest of cleanup ...
    }
}
```

---

## Performer's Task Scheduling Another Performer

### Scenario Analysis

**Example:**
```cpp
// Thread 1: RunLoop A
performerA->perform([performerB]() {
    // This runs on RunLoop A's thread
    performerB->perform([]() {
        // This will run on RunLoop B's thread
    });
});
```

**What Happens:**
1. Task from `performerA` executes on **RunLoop A's thread**
2. Task calls `performerB->perform()` from RunLoop A's thread
3. Task is queued in `performerB`'s queue
4. Pipe write signals RunLoop B
5. When RunLoop B processes it, task executes on **RunLoop B's thread**

**Is This Safe?**
- **Yes**, as long as both Performers and RunLoops are still alive
- The task from `performerA` has a copy of the lambda/capture, so it can safely call `performerB->perform()`
- Cross-thread scheduling works correctly

**Potential Issues:**

1. **If Performer A is destroyed while its task is running:**
   ```cpp
   performerA->perform([performerB]() {
       // Task is executing...
       // Performer A is destroyed here
       performerB->perform([]() { ... });  // Is this safe?
   });
   ```
   - **Answer:** Yes, it's safe! The task has a copy of `performerB` (via capture)
   - The task doesn't access `performerA` after it starts executing
   - `performerB` is still valid (assuming it's managed separately)

2. **If RunLoop B is destroyed:**
   ```cpp
   performerA->perform([performerB, runLoopB]() {
       // Task executes on RunLoop A's thread
       performerB->perform([]() { ... });  // RunLoop B destroyed?
   });
   ```
   - **Answer:** This is **dangerous**! If RunLoop B is destroyed:
     - `performerB->perform()` may succeed in queuing the task
     - But the task will never execute (RunLoop B isn't running)
     - If `wait=true`, it will wait forever

3. **If Performer B is destroyed:**
   ```cpp
   performerA->perform([performerB]() {
       performerB->perform([]() { ... });  // Performer B destroyed?
   });
   ```
   - **Answer:** This is **safe**! `perform()` checks `m_closed`:
     ```cpp
     void Performer::perform(const Task &task, bool wait) {
         if(m_closed)
             return;  // Silently ignores
     }
     ```
   - Task is simply not queued (silent failure)

---

## Summary

### Thread Context
- ✅ **Tasks execute on the RunLoop's thread**, not the calling thread
- ✅ Exception: If `wait=true` and called from RunLoop thread, executes immediately

### Performer Destruction Before RunLoop
- ❌ **Bug:** Tasks queued before `close()` are **not executed** (silently dropped)
- ❌ **Bug:** If `wait=true`, waiting threads may deadlock
- ⚠️ **Fix needed:** Reorder `close()` to execute tasks before setting `m_closed`

### Task Scheduling Another Performer
- ✅ **Safe:** Tasks can schedule other Performers
- ✅ **Safe:** Tasks execute on their respective RunLoop threads
- ⚠️ **Caution:** Ensure target RunLoop is still running
- ⚠️ **Caution:** If target Performer is closed, task is silently ignored

### Best Practices

1. **Always close Performers before destroying RunLoops:**
   ```cpp
   performer->close();
   runLoop->clear();
   ```

2. **Don't rely on tasks executing during destruction:**
   - Tasks queued before `close()` may not execute
   - Use explicit cleanup instead

3. **When scheduling cross-Performer:**
   - Ensure target RunLoop is running
   - Handle case where target Performer might be closed
   - Consider using `shared_ptr` to ensure lifetime

4. **For sync performs:**
   - Be aware of potential deadlocks
   - Don't wait on a Performer whose RunLoop isn't running
   - Don't create circular wait dependencies

---

## Recommended Fixes

### Fix 1: Execute Tasks During Close

```cpp
void Performer::close() {
    if(not m_closed) {
        m_closed = true;
        
        // Execute all queued tasks before cleanup
        while(true) {
            std::shared_ptr<Item> each;
            {
                std::unique_lock<std::mutex> locked(m_mutex);
                if(m_items.empty())
                    break;
                each = m_items.front();
                m_items.pop();
            }
            each->fire();  // Execute even though m_closed is true
        }
        
        if(m_runLoop)
            m_runLoop->unregisterDescriptor(m_pipe[READ_PIPE_IDX]);
        m_runLoop = nullptr;
        ::close(m_pipe[READ_PIPE_IDX]);
        ::close(m_pipe[WRITE_PIPE_IDX]);
    }
}
```

### Fix 2: Add Null Check in perform()

```cpp
void Performer::perform(const Task &task, bool wait) {
    if(m_closed or not m_runLoop)  // Add null check
        return;
    // ... rest of code
}
```

### Fix 3: Document Thread Safety

Add documentation clarifying:
- Tasks execute on RunLoop thread
- Destruction order requirements
- Cross-Performer scheduling behavior

