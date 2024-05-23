# GuardFW

Templated C++ wrapper around Linux API functions for proper error handling.

(c) Simon Gleissner <simon@gleissner.de>, http://guardfw.de

released under the MIT license, see file LICENSE

## Introduction

GuardFW is a framework for using POSIX and other Linux API system calls in a secured way:

- It provides a highly configurable wrapper template, which expands the system calls with an obligatory error
  interface. Hard errors (which applies for most errors) are normally thrown as std::system_error exceptions. Other
  errors are reported or handled directly.
- The way how errors are detected, reported and handled is configurable in call contexts. Using the right context
  depends on the corresponding API system call, its predefined error handling and special meaning of selected error
  codes.
- If possible, the return value is changed to a type, which makes sense for success cases only.
- To make the usage easy, it is intended to provide at least one predefined, wrapped call for each system API call. The
  goal is to hide the complex template wrapper. It shall use the same or a similar name with the same arguments, but in
  the namespace GuardFW::. Using the wrappers instead of direct API calls allows the programmer to concentrate on the
  functionality. Pointers to single objects may be exchanged against references, if they are forbidden to be nullptr.
- Additionally, logical connected functions are combined in Guards, which prevent some misusage. Guard constructors open
  kernel objects, destructors close them, member functions use them.
- This is a Linux-only library. It is neither portable to other operating systems, nor is this planned.
- GuardFW requires C++23 (currently not yet released as ISO standard) and will switch from GCC-12 to GCC-13 (as soon
  GitHub runners do support it, which may be after its official release in spring 2023). Sorry for that. Unit tests are
  done with Catch2.

## Some Examples

### close()

The POSIX call `int close(int fd)` has some anomalies, which are not directly obvious:

- It does not have a useful success return value. It indicates only if an error has occured, but if hard errors are
  always thrown as exceptions, a wrapper might return void.
- Additionally, the error `EINTR` might be returned in case of received signals, but especially under Linux it is
  guaranteed that the file will be closed, so this error can be ignored.

The wrapper call for `close()` in this library looks like this:

```
inline static void close(
	FileDescriptor fd,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextIgnoreEINTR::wrapper<::close, void>(source_location, fd);
}
```

As you can see:

- the wrapper `GuardFW::close` returns `void`. Either the call throws or just succeeds.
- `FileDescriptor` is defined as `int`, but choosing a distinct type name improves readability a bit.
- The `source_location` object reference shall be used in case of an exception. As the wrappers are inlined, hopefully
  its first access is moved into the exception part of the wrapper and shall not slow down the execution, even if it is
  only a fast pointer operation to a compiler-generated constant.
- The context `ContextIgnoreEINTR` is a typedef to the more generic context template
  `Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::ignore_softerrors, EINTR>`, which
  basically means:
  - `ErrorIndication::eqm1_errno`: The return value `-1` indicates an error, which is returned in `errno`.
  - `ErrorReport::exception` Errors shall be thrown.
  - There is a list of 'soft' errors appended (here: `EINTR`), which shall be handled in a different way.
  - `ErrorSpecial::ignore_softerrors` The soft errors shall be ignored, as `EINTR` is not a real error here.
- The first template parameters of the `wrapper<>()` call contains the POSIX API function. With the correct context, all
  relevant errors of this function will be thrown.
- The optional second template parameter describes the success return type. Normally it would be auto-deducted as `int`,
  as `::close` returns `int`, but here the return value is only used for error indication, so the return type is
  overridden with `void`.

Usage:

```
GuardFW::close(fd);
```

Yes, it's as simple, even or because of the wrapper. If you made a previous mistake with the file descriptor, it will
throw and you will know, but this would mean that there is an error somewhere else in your program. If your hard drive
is broken, it will throw. If your NFS storage if full and the closed file somehow corrupt, it will throw. Else it will
just succeed without any usable return value.

### read()

`ssize_t read(int fd, void* buf, size_t count)` has three special behaviors:

- The success value should be `size_t` (the number of bytes read), but the negative sign bit is necessary to be capable
  of returning `-1` in case of an error.
- `read()` may be interrupted by a signal and returns `EINTR`. In that case, the command shall be repeated.
- In case of nonblocking/asynchronous calls, it may return `EAGAIN` or `EWOULDBLOCK` (which is the same under Linux).
  This is not an error, but we also don't have a usable return value.

This is the wrapper call in a non-blocking environment:

```
[[nodiscard]] inline static std::optional<size_t> read_nonblock(
	FileDescriptor fd,
	void* buf,
	size_t count,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextNonblockRepeatEINTR::wrapper<::read, size_t>(source_location, fd, buf, count);
}
```

Notes for the wrapper:

- The return type is `std::optional<size_t>`, which can't be ignored. In case of a detected blocking, it will be
  empty, else it will contain the casted `size_t` of the read block.
- The context `ContextNonblockRepeatEINTR` is an alias for
  `Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats|ErrorSpecial::nonblock>`:
  - `ErrorIndication::eqm1_errno`: Errors are indicated by returning `-1` and reported in `errno`.
  - `ErrorReport::exception`: Errors are thrown.
  - `ErrorSpecial::eintr_repeats`: The error `EINTR` indicates a signal, which interrupted the call. `read()` shall be
    repeated instead of throwing an exception.
  - `ErrorSpecial::nonblock`: `EAGAIN` or `EWOULDBLOCK` reports a prevented blocking. The call will return `nullopt`
    in this case as a blocking result.
- The internal `wrapper<:read, size_t>(...)` call will cast the success return value from a `ssize_t` to a `size_t`.

Usage:

```
auto result = GuardFW::read_nonblock(fd, buf, count);
if(result)  // operator bool
    // success, do something with result.value() or *result
else
    // read would have blocked
```

Again, all relevant hard errors are thrown and the rest is handled internally.
