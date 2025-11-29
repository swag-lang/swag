/**

# Intrinsics in Swag

Intrinsics are built-in functions provided by the Swag compiler that offer low-level operations, often directly mapping to
specific machine instructions or providing essential compiler utilities. All intrinsics in Swag are prefixed with '@',
which is reserved exclusively for these functions.

This document provides a categorized list of all intrinsics available in Swag.
*/

#global skip

/**

# Base Intrinsics

Fundamental intrinsics commonly needed across Swag programs.
*/

func @assert(value: bool);                       // Assert that a condition is true (debugging).
func @breakpoint();                              // Trigger a debugger breakpoint.
func @getcontext() -> *Swag.Context;             // Retrieve the current execution context.
func @setcontext(context: const *Swag.Context);  // Set the current execution context.

@panic();                                        // Trigger a panic, stopping program execution.
@compilererror();                                // Generate a compile-time error.
@compilerwarning();                              // Generate a compile-time warning.

/**

# Value Intrinsics

*/

@err:        any                  // The current raised error (null if none).
@args:       const [..] string    // Command-line arguments passed to the program.
@bytecode:   bool                 // True if the code is being executed as bytecode.
@pinfos:     *Swag.ProcessInfos   // Retrieve program information.
@compiler:   Swag.ICompiler       // Retrieve the current compiler interface.
@rtflags:
@index:
@modules:
@gvtd:

/**

# Built-in Intrinsics

Essential operations related to type and memory management for low-level or performance-critical code.
*/

@init();                        // Initialize a variable or memory area.
@drop();                        // Destroy a variable or memory area.
@postmove();                    // Post-move hook.
@postcopy();                    // Post-copy hook.
@kindof();                      // Kind of a type (e.g., primitive, struct).
@countof();                     // Number of elements in an array.
@dataof();                      // Pointer to underlying data of a type.
@mkslice();                     // Create a slice from a data pointer and length.
@mkstring();                    // Create a string from a data pointer and length.
@mkany();                       // Create a generic 'any' from a value.
@mkinterface();                 // Create an interface from an implementation.
@mkcallback();                  // Create a callback from a function pointer.
@tableof();                     // Interface table for a given type.

#sizeof();                      // Size in bytes of a type or variable.
#alignof();                     // Alignment requirement of a type.
#offsetof();                    // Offset in bytes of a struct field.
#typeof();                      // Type of an expression.
#stringof();                    // String representation of a type or expression.
#isconstexpr();                 // Check if an expression is a constant expression.

/**

# Memory Intrinsics

Memory management operations for allocation, deallocation, and manipulation.
*/

func @alloc(size: u64) -> *void;                         // Allocate a block of memory.
func @realloc(ptr: *void, size: u64) -> *void;           // Reallocate a block of memory.
func @free(ptr: *void);                                  // Free a previously allocated block.
func @memset(dst: *void, value: u8, size: u64);          // Set a block of memory to a value.
func @memcpy(dst: *void, src: const *void, size: u64);   // Copy a block of memory.
func @memmove(dst: *void, src: const *void, size: u64);  // Move a block of memory (overlap-safe).
func @memcmp(dst: const *void, src: const *void, size: u64) -> s32; // Compare two blocks of memory.
func @strlen(value: const *u8) -> u64;                   // Length of a null-terminated string.

/**

# Atomic Intrinsics

Thread-safe manipulation of variables in shared memory.
*/

func @atomadd(addr: *s8,  value: s8)  -> s8;
func @atomadd(addr: *s16, value: s16) -> s16;
func @atomadd(addr: *s32, value: s32) -> s32;
func @atomadd(addr: *s64, value: s64) -> s64;
func @atomadd(addr: *u8,  value: u8)  -> u8;
func @atomadd(addr: *u16, value: u16) -> u16;
func @atomadd(addr: *u32, value: u32) -> u32;
func @atomadd(addr: *u64, value: u64) -> u64;

func @atomand(addr: *s8,  value: s8)  -> s8;
func @atomand(addr: *s16, value: s16) -> s16;
func @atomand(addr: *s32, value: s32) -> s32;
func @atomand(addr: *s64, value: s64) -> s64;
func @atomand(addr: *u8,  value: u8)  -> u8;
func @atomand(addr: *u16, value: u16) -> u16;
func @atomand(addr: *u32, value: u32) -> u32;
func @atomand(addr: *u64, value: u64) -> u64;

func @atomor(addr: *s8,  value: s8)  -> s8;
func @atomor(addr: *s16, value: s16) -> s16;
func @atomor(addr: *s32, value: s32) -> s32;
func @atomor(addr: *s64, value: s64) -> s64;
func @atomor(addr: *u8,  value: u8)  -> u8;
func @atomor(addr: *u16, value: u16) -> u16;
func @atomor(addr: *u32, value: u32) -> u32;
func @atomor(addr: *u64, value: u64) -> u64;

func @atomxor(addr: *s8,  value: s8)  -> s8;
func @atomxor(addr: *s16, value: s16) -> s16;
func @atomxor(addr: *s32, value: s32) -> s32;
func @atomxor(addr: *s64, value: s64) -> s64;
func @atomxor(addr: *u8,  value: u8)  -> u8;
func @atomxor(addr: *u16, value: u16) -> u16;
func @atomxor(addr: *u32, value: u32) -> u32;
func @atomxor(addr: *u64, value: u64) -> u64;

func @atomxchg(addr: *s8,  exchangeWith: s8)  -> s8;
func @atomxchg(addr: *s16, exchangeWith: s16) -> s16;
func @atomxchg(addr: *s32, exchangeWith: s32) -> s32;
func @atomxchg(addr: *s64, exchangeWith: s64) -> s64;
func @atomxchg(addr: *u8,  exchangeWith: u8)  -> u8;
func @atomxchg(addr: *u16, exchangeWith: u16) -> u16;
func @atomxchg(addr: *u32, exchangeWith: u32) -> u32;
func @atomxchg(addr: *u64, exchangeWith: u64) -> u64;

func @atomcmpxchg(addr: *s8,  compareTo: s8,  exchangeWith: s8)  -> s8;
func @atomcmpxchg(addr: *s16, compareTo: s16, exchangeWith: s16) -> s16;
func @atomcmpxchg(addr: *s32, compareTo: s32, exchangeWith: s32) -> s32;
func @atomcmpxchg(addr: *s64, compareTo: s64, exchangeWith: s64) -> s64;
func @atomcmpxchg(addr: *u8,  compareTo: u8,  exchangeWith: u8)  -> u8;
func @atomcmpxchg(addr: *u16, compareTo: u16, exchangeWith: u16) -> u16;
func @atomcmpxchg(addr: *u32, compareTo: u32, exchangeWith: u32) -> u32;
func @atomcmpxchg(addr: *u64, compareTo: u64, exchangeWith: u64) -> u64;

/**

# Math Intrinsics

Mathematical operations including trigonometric, logarithmic, rounding, and bit utilities.
*/

func @sqrt(value: f32) -> f32;
func @sqrt(value: f64) -> f64;

func @sin(value: f32) -> f32;
func @sin(value: f64) -> f64;
func @cos(value: f32) -> f32;
func @cos(value: f64) -> f64;
func @tan(value: f32) -> f32;
func @tan(value: f64) -> f64;

func @sinh(value: f32) -> f32;
func @sinh(value: f64) -> f64;
func @cosh(value: f32) -> f32;
func @cosh(value: f64) -> f64;
func @tanh(value: f32) -> f32;
func @tanh(value: f64) -> f64;

func @asin(value: f32) -> f32;
func @asin(value: f64) -> f64;
func @acos(value: f32) -> f32;
func @acos(value: f64) -> f64;
func @atan(value: f32) -> f32;
func @atan(value: f64) -> f64;

func @log(value: f32) -> f32;
func @log(value: f64) -> f64;
func @log2(value: f32) -> f32;
func @log2(value: f64) -> f64;
func @log10(value: f32) -> f32;
func @log10(value: f64) -> f64;

func @floor(value: f32) -> f32;
func @floor(value: f64) -> f64;
func @ceil(value: f32) -> f32;
func @ceil(value: f64) -> f64;
func @trunc(value: f32) -> f32;
func @trunc(value: f64) -> f64;
func @round(value: f32) -> f32;
func @round(value: f64) -> f64;

func @abs(value: s8)  -> s8;
func @abs(value: s16) -> s16;
func @abs(value: s32) -> s32;
func @abs(value: s64) -> s64;
func @abs(value: f32) -> f32;
func @abs(value: f64) -> f64;

func @exp(value: f32) -> f32;
func @exp(value: f64) -> f64;
func @exp2(value: f32) -> f32;
func @exp2(value: f64) -> f64;
func @pow(value1: f32, value2: f32) -> f32;
func @pow(value1: f64, value2: f64) -> f64;

func @min(value1: s8,  value2: s8)  -> s8;
func @min(value1: s16, value2: s16) -> s16;
func @min(value1: s32, value2: s32) -> s32;
func @min(value1: s64, value2: s64) -> s64;
func @min(value1: u8,  value2: u8)  -> u8;
func @min(value1: u16, value2: u16) -> u16;
func @min(value1: u32, value2: u32) -> u32;
func @min(value1: u64, value2: u64) -> u64;
func @min(value1: f32, value2: f32) -> f32;
func @min(value1: f64, value2: f64) -> f64;

func @max(value1: s8,  value2: s8)  -> s8;
func @max(value1: s16, value2: s16) -> s16;
func @max(value1: s32, value2: s32) -> s32;
func @max(value1: s64, value2: s64) -> s64;
func @max(value1: u8,  value2: u8)  -> u8;
func @max(value1: u16, value2: u16) -> u16;
func @max(value1: u32, value2: u32) -> u32;
func @max(value1: u64, value2: u64) -> u64;
func @max(value1: f32, value2: f32) -> f32;
func @max(value1: f64, value2: f64) -> f64;

func @bitcountnz(value: u8)  -> u8;
func @bitcountnz(value: u16) -> u16;
func @bitcountnz(value: u32) -> u32;
func @bitcountnz(value: u64) -> u64;
func @bitcounttz(value: u8)  -> u8;
func @bitcounttz(value: u16) -> u16;
func @bitcounttz(value: u32) -> u32;
func @bitcounttz(value: u64) -> u64;
func @bitcountlz(value: u8)  -> u8;
func @bitcountlz(value: u16) -> u16;
func @bitcountlz(value: u32) -> u32;
func @bitcountlz(value: u64) -> u64;

func @byteswap(value: u16) -> u16;
func @byteswap(value: u32) -> u32;
func @byteswap(value: u64) -> u64;

func @rol(value: u8,  num: u8)  -> u8;
func @rol(value: u16, num: u16) -> u16;
func @rol(value: u32, num: u32) -> u32;
func @rol(value: u64, num: u64) -> u64;

func @ror(value: u8,  num: u8)  -> u8;
func @ror(value: u16, num: u16) -> u16;
func @ror(value: u32, num: u32) -> u32;
func @ror(value: u64, num: u64) -> u64;

func @muladd(val1: f32, val2: f32, val3: f32) -> f32;
func @muladd(val1: f64, val2: f64, val3: f64) -> f64;
