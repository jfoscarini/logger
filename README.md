# Header-only Logger / Scope Profiler for C++

A lightweight, header-only C++ logging utility with colored output, stream-style syntax, and optional scope profiling.

## Features

- Header-only
- Stream-like usage: `log_info() << "message";` or `log_info(category) << "message";`
- Color-coded log levels using ANSI escape sequences
- Scope-based profiling with automatic timing
- Source location metadata via `std::source_location`
- Zero runtime overhead in release builds

## Requirements

- C++20 or later
- Terminal that supports ANSI colors (most Unix-like terminals)

## Example

```cpp
#include "logger.hpp"

int main() {
    log_info() << "Starting application...";
    {
        log_profile(init_code);
        // some initialization code
    }
    log_warning(DISK) << "Low disk space.";
    log_error(DISK) << "Failed to open file.";
}
```

## Example Output

```
[12:00:00.000][  INFO   ] Starting application...
[18:00:00.000][PROFILING] START init_code @ main.cpp:6
[18:00:00.000][PROFILING] FINISH init_code (0.062ms) @ main.cpp:6
[18:00:00.000][ WARNING ][DISK] Low disk space.
[18:00:00.000][  ERROR  ][DISK] Failed to open file.
```

## Profiling Scopes

Use the `profile(name)` macro to time a scope. Automatically logs on entry and exit:

```cpp
void heavy_task() {
    log_profile(heavy_task);
    // your code
}
```

## Log Levels and Colors

| Level     | Color                      |
| --------- | -------------------------- |
| Trace     | Light Gray (Bold)          |
| Debug     | Blue (Bold)                |
| Info      | Green (Bold)               |
| Notice    | Cyan (Bold)                |
| Warning   | Yellow (Bold)              |
| Error     | Red (Bold)                 |
| Critical  | Magenta (Bold)             |
| Alert     | Red Background (Bold)      |
| Emergency | Red BG + White Text (Bold) |
| Profile   | Cyan (Bold)                |

## Installation

Just copy `logger.hpp` to your project and include it:

```cpp
#include "logger.hpp"
```

No dependencies, no linking.

## Release Mode

To disable logging and profiling macros in release builds, define `NDEBUG`:

```bash
g++ -DNDEBUG -std=c++20 main.cpp
```

By defining `NDEBUG` (usually done automatically in release builds), all log macros become no-ops with zero runtime overhead:

```cpp
#ifdef NDEBUG
#define log_info() if(true) {} else Logger(...)
#else
#define log_info() Logger(...)
#endif
```
