# Performer Quick Reference

## One-Line Summary

`Performer` allows any thread to safely execute code on a `RunLoop`'s thread using a pipe-based signaling mechanism.

## Key Facts

| Aspect | Answer |
|--------|--------|
| **Thread Context** | Tasks execute on **RunLoop's thread**, not calling thread |
| **Thread Safety** | `perform()` is thread-safe, can be called from any thread |
| **Execution Order** | Tasks execute in FIFO order |
| **Lifetime** | Performer must outlive RunLoop (or close before RunLoop destruction) |
| **Current Bug** | Tasks queued before `close()` are not executed |

## Basic Usage

```cpp
// Setup
PreferredRunLoop loop;
Performer performer(&loop);

// Async perform (from any thread)
performer.perform([]() {
    // Runs on RunLoop thread
});

// Sync perform (blocks until complete)
performer.perform([]() {
    // Runs on RunLoop thread
}, true);

// Cleanup
performer.close();
loop.clear();
```

## Thread Context Cheat Sheet

```cpp
// Thread A
performer.perform([]() {
    // Executes on RunLoop thread (not Thread A)
});

// RunLoop thread
performer.perform([]() {
    // Executes immediately on RunLoop thread (if wait=true)
}, true);
```

## Common Patterns

### Worker to Main Thread
```cpp
std::thread worker([&performer]() {
    auto result = compute();
    performer.perform([result]() {
        updateUI(result);  // Main thread
    });
});
```

### Synchronous Cross-Thread
```cpp
std::atomic<int> value{0};
performer.perform([&value]() {
    value = getValue();
}, true);  // Wait for completion
// value is now set
```

### Nested Performer
```cpp
performer1.perform([&performer2]() {
    // Runs on RunLoop 1 thread
    performer2.perform([]() {
        // Runs on RunLoop 2 thread
    });
});
```

## Destruction Order

```cpp
// ✅ CORRECT
performer->close();  // Execute queued tasks
loop->clear();       // Clear timers/descriptors

// ❌ WRONG
delete performer;    // Tasks lost!
loop->clear();
```

## Safety Checklist

- [ ] Performer outlives RunLoop (or close before destruction)
- [ ] RunLoop is running when using sync performs
- [ ] No circular wait dependencies
- [ ] Tasks don't assume Performer lifetime
- [ ] Proper cleanup order

## Known Issues

1. **Tasks not executed during close()** - Workaround: Ensure tasks execute before close
2. **No null check for RunLoop** - Workaround: Ensure RunLoop outlives Performer
3. **Pipe write failure ignored** - Workaround: Monitor task execution

## See Also

- [Comprehensive Documentation](performer-comprehensive.md)
- [Lifecycle Analysis](performer-lifecycle-analysis.md)
- [Bug Analysis](performer-bug-analysis.md)

