Memory and Ownership
====================

Reference‑Counted Base: `Object`

- Intrusive reference counting with `retain()` and `release()`; default copy is disabled.
- Static helpers: `Object::retain(obj)`, `Object::release(obj)`.
- Utility: `share_ref(T* ptr, bool retain=true)` wraps an `Object` in `std::shared_ptr` that forwards to `retain/release`.

Retainer<T>

- A lightweight smart holder akin to `std::shared_ptr<T>` but calling `T::retain()`/`T::release()`.
- Constructors allow ownership claim without an extra retain: `Retainer(T* ptr, bool retain)` or `claim_ref(new T)`.
- Assignment and move semantics are supported; cross‑type conversion constructors allow `Retainer<Derived>` to `Retainer<Base>`.

Choosing Between `std::shared_ptr` and `Retainer`

- Use `Retainer<T>` when you want intrusive semantics and zero allocator overhead for the control block.
- Use `std::shared_ptr<T>` when interoperating with APIs expecting `shared_ptr` or for weak pointer needs.
- `share_ref()` offers a bridge: keep intrusive ownership on the inside while exposing `shared_ptr` to callers.

Patterns

Creating and sharing an intrusive object

```cpp
#include <zenomt/Object.hpp>

using namespace com::zenomt;

class Thing : public Object {
public:
  void doIt();
};

auto makeThingForAPI() {
  auto raw = new Thing();              // refcount = 1 (by construction contract)
  return share_ref(raw, /*retain=*/false); // expose as shared_ptr without extra retain
}
```

Using `Retainer` to manage lifetimes

```cpp
#include <zenomt/Retainer.hpp>

using namespace com::zenomt;

Retainer<Thing> held = claim_ref(new Thing()); // take ownership of initial ref
if (held) {
  held->doIt();
}
held.reset(); // releases
```

Guidelines

- Objects are intended for pointer‑based use; avoid copying instances.
- Avoid retain/release imbalances; prefer `Retainer` or `share_ref` to manual calls.
- Never call `delete` directly on an `Object` subclass; the final `release()` will do that.


