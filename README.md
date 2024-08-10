# FunctionBuffer

## Overview
`FunctionBuffer` is a C++ library providing an alternative to `std::function` that uses a fixed-size buffer for storing callable objects. Designed for embedded systems and real-time applications, `FunctionBuffer` eliminates heap allocations, ensuring predictable memory usage and performance.

## Features
- **Fixed-Size Buffer:** Stores callable objects in a preallocated, stack-based buffer, avoiding dynamic memory allocation.
- **Configurable Buffer Size:** Customize the buffer size via a template parameter to fit your specific needs.
- **Supports Various Callables:** Works with function pointers, lambdas, and functors.
- **Optimized for Performance:** Minimal overhead and suitable for real-time and memory-constrained environments.
