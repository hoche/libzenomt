# Performer Bug Analysis: Edge Cases

**Date:** 2024

## Overview

Analysis of two critical edge cases in the `Performer` class that could lead to crashes, hangs, or undefined behavior in production code.

---

## Scenario 1: RunLoop Destroyed Before Performer's Task Runs

### Critical Issues

#### 1. Use-After-Free in `perform()`

**Location:** `src/Performer.cpp:90`

```cpp
if(wait and m_runLoop->isRunningInThisThread())
```

**Problem:**
- `Performer` stores a raw pointer `RunLoop *m_runLoop` (line 27)
- If the RunLoop is destroyed before `perform()` is called, `m_runLoop` becomes a dangling pointer
- Dereferencing it causes undefined behavior (use-after-free)

**Impact:** Crash or memory corruption

#### 2. Queued Tasks Never Execute

**Location:** `src/Performer.cpp:127-144` (`fireItems()`)

**Problem:**
- Tasks queued before RunLoop destruction remain in `m_items` queue
- `onSignaled()` callback won't fire because RunLoop isn't running
- Tasks only execute if `close()` is explicitly called, which calls `fireItems()`
- If RunLoop is destroyed first, `close()` may not be called, leaving tasks orphaned

**Impact:** Tasks silently lost, potential resource leaks

#### 3. Callback Invalidation

**Location:** `src/Performer.cpp:61`

```cpp
m_runLoop->registerDescriptor(m_pipe[READ_PIPE_IDX], RunLoop::READABLE, 
    [this] { this->onSignaled(); });
```

**Problem:**
- Lambda captures `this` pointer
- If RunLoop destructor doesn't clean up descriptors, callback could be invoked on destroyed `Performer`
- No check if `Performer` is still valid when callback fires

**Impact:** Crash or undefined behavior

#### 4. Silent Pipe Write Failure

**Location:** `src/Performer.cpp:108`

```cpp
(void)write(m_pipe[WRITE_PIPE_IDX], buf, sizeof(buf));
```

**Problem:**
- Return value ignored
- If RunLoop thread exits, read end of pipe may be closed
- `write()` fails with `EPIPE`, but error is silently ignored
- Task may never be signaled

**Impact:** Tasks may not execute

### What Actually Happens

1. **If `perform()` called after RunLoop destruction:**
   - **Result:** Undefined behavior (use-after-free crash)

2. **If tasks queued before destruction:**
   - **Result:** Tasks remain queued, never execute unless `close()` called

3. **If `onSignaled()` invoked after destruction:**
   - **Result:** Undefined behavior (dangling `this` pointer)

---

## Scenario 2: Performer's Task Schedules Another Performer

### How It Works

Looking at `test/testperform.cpp:30-40`, nested performs are used:

```cpp
workerPerformer->perform([=] {
    workerRL->scheduleRel(Timer::makeAction([=] (Time now) {
        performer->perform([] { ... });  // Async - OK
        performer->perform([...], true);  // Sync - potential issue
    }), 1, 1);
});
```

### Async Performs (`wait=false`)

**Behavior:**
- Task queues new item and writes to pipe
- Target RunLoop processes it in next cycle
- **Status:** ✅ Works correctly

### Sync Performs (`wait=true`)

**Behavior:**
- If called from RunLoop thread: executes immediately (lines 90-95)
- If called from different thread: waits for completion (lines 111-116)

**Potential Deadlock:**

```cpp
// Thread 1 (RunLoop A)
performerB->perform([=] {
    performerA->perform([=] { ... }, true);  // Waits for Thread 2
}, true);  // Waits for Thread 1

// Thread 2 (RunLoop B)  
performerA->perform([=] {
    performerB->perform([=] { ... }, true);  // Waits for Thread 1
}, true);  // Waits for Thread 2
```

**Result:** Deadlock - both threads wait indefinitely

### Same-Performer Recursion

**Behavior:**
- Task can call `perform()` on same Performer
- New task is queued and runs in next cycle
- **Status:** ✅ No immediate recursion issues

### Potential Issues

#### 1. Deadlock Risk

**Scenario:**
- Two threads with cross-referencing Performers
- Both call sync `perform()` on each other's Performer
- **Result:** Deadlock

#### 2. RunLoop Not Running

**Scenario:**
- Task calls `perform(..., true)` on Performer whose RunLoop isn't running
- **Result:** Waits indefinitely

#### 3. Task Ordering

**Behavior:**
- Tasks scheduled from within a task execute in next cycle, not immediately
- **Status:** ✅ Expected behavior, but may be surprising

---

## Recommendations

### For Scenario 1 (RunLoop Destruction)

#### 1. Add Null Check in `perform()`

```cpp
void Performer::perform(const Task &task, bool wait)
{
    if(m_closed)
        return;
    
    if(not m_runLoop)  // ADD THIS CHECK
        return;
    
    if(wait and m_runLoop->isRunningInThisThread())
    {
        // ...
    }
}
```

#### 2. Use Weak Reference for RunLoop

**Option A:** Store `std::weak_ptr<RunLoop>` instead of raw pointer
- Check validity before use
- Automatically handles destruction

**Option B:** Use `std::shared_ptr<RunLoop>`
- Ensures RunLoop lives as long as Performer
- May extend lifetime unnecessarily

#### 3. Ensure Cleanup Order

- Document that `Performer::close()` must be called before RunLoop destruction
- Or make RunLoop unregister all descriptors in destructor

### For Scenario 2 (Nested Performer)

#### 1. Document Deadlock Risks

- Add documentation warning about circular dependencies
- Provide examples of safe patterns

#### 2. Add Timeout for Sync Performs

```cpp
void Performer::perform(const Task &task, bool wait, Duration timeout = 5.0)
{
    // ... existing code ...
    
    if(wait)
    {
        std::unique_lock<std::mutex> locked(*item->m_mutex);
        auto deadline = std::chrono::steady_clock::now() + 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::duration<Duration>(timeout));
        
        if(not item->m_cond->wait_until(locked, deadline))
        {
            // Timeout - handle error
            return;
        }
    }
}
```

#### 3. Add Assertions

```cpp
void Performer::perform(const Task &task, bool wait)
{
    if(wait and m_runLoop and not m_runLoop->isRunningInThisThread())
    {
        // Assert that RunLoop is actually running
        assert(m_runLoop->isRunningInThisThread() or 
               "Sync perform requires RunLoop to be running");
    }
}
```

### General Improvements

#### 1. Check `write()` Return Value

```cpp
if(not m_signaled)
{
    uint8_t buf[1] {};
    m_signaled = true;
    ssize_t rv = write(m_pipe[WRITE_PIPE_IDX], buf, sizeof(buf));
    if(rv < 0)
    {
        // Handle error (EPIPE, EAGAIN, etc.)
        m_signaled = false;
        // Log or notify error
    }
}
```

#### 2. Add Null Checks

- Check `m_runLoop` before all uses
- Add defensive programming throughout

#### 3. Lifecycle Management

- Consider making `m_runLoop` a `std::weak_ptr` or adding lifecycle management
- Ensure proper cleanup order

---

## Summary

### Severity Assessment

| Issue | Severity | Impact |
|-------|----------|--------|
| Use-after-free in `perform()` | **CRITICAL** | Crash, memory corruption |
| Queued tasks never execute | **HIGH** | Silent failures, resource leaks |
| Callback invalidation | **CRITICAL** | Crash, undefined behavior |
| Silent pipe write failure | **MEDIUM** | Tasks may not execute |
| Deadlock in nested performs | **HIGH** | Application hang |
| Infinite wait if RunLoop not running | **MEDIUM** | Application hang |

### Priority Fixes

1. **Immediate:** Add null check for `m_runLoop` in `perform()`
2. **High:** Fix use-after-free issues
3. **High:** Add error handling for `write()` return value
4. **Medium:** Document deadlock risks
5. **Medium:** Add timeout for sync performs**

These are real bugs that could cause crashes or hangs in production code. The fixes should be prioritized based on the severity assessment above.

