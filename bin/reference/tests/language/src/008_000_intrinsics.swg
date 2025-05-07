/**
# Intrinsics in Swag
Intrinsics are built-in functions provided by the Swag compiler that offer low-level operations, often directly mapping to
specific machine instructions or providing essential compiler utilities. All intrinsics in Swag are prefixed with `@`,
which is reserved exclusively for these functions.

This document provides a categorized list of all intrinsics available in Swag.
*/

#global skip

/**
# Base Intrinsics
These base intrinsics provide fundamental functionalities that are commonly needed across various Swag programs.
*/

func @assert(value: bool);                       // Asserts that a condition is true, used for debugging.
func @breakpoint();                              // Triggers a breakpoint in the debugger.
func @getcontext() -> *Swag.Context;             // Retrieves the current execution context.
func @setcontext(context: const *Swag.Context);  // Sets the current execution context.
func @compiler() -> Swag.ICompiler;              // Retrieves the current compiler interface.

@panic()                                         // Triggers a panic, stopping program execution.
@compilererror()                                 // Generates a compile-time error.
@compilerwarning()                               // Generates a compile-time warning.

/**
# Values Intrinsics
*/

@err:        any                    // The current raised error (null if none)
@args:       const [..] string      // The command-line arguments passed to the program.
@isbytecode: bool                   // True if the code is being executed as bytecode

/**
# Built-in Intrinsics
These intrinsics provide essential built-in operations related to type and memory management, typically for
low-level or performance-critical code.
*/

@init()                     // Initializes a variable or memory area.
@drop()                     // Destroys a variable or memory area.
@postmove()                 // Called after a move operation to handle post-move logic.
@postcopy()                 // Called after a copy operation to handle post-copy logic.
#sizeof()                   // Returns the size, in bytes, of a type or variable.
#alignof()                  // Returns the alignment requirement of a type.
#offsetof()                 // Returns the offset, in bytes, of a field within a struct.
#typeof()                   // Returns the type of a given expression.
@kindof()                   // Returns the kind (e.g., primitive, struct) of a type.
@countof()                  // Returns the number of elements in an array.
#stringof()                 // Returns the string representation of a type or expression.
@dataof()                   // Returns a pointer to the underlying data of a type.
@mkslice()                  // Creates a slice from a given data pointer and length.
@mkstring()                 // Creates a string from a given data pointer and length.
@mkany()                    // Creates a generic `any` type from a given value.
@mkinterface()              // Creates an interface type from a given implementation.
@mkcallback()               // Creates a callback from a given function pointer.
@pinfos()                   // Retrieves program information.
#isconstexpr()              // Checks if an expression is a constant expression.
@itftableof()               // Returns the interface table for a given type.

/**
# Memory-related Intrinsics
These intrinsics offer memory management operations, allowing for fine-grained control over memory allocation,
deallocation, and manipulation.
*/

func @alloc(size: u64) -> *void;                         // Allocates a block of memory of the given size.
func @realloc(ptr: *void, size: u64) -> *void;           // Reallocates a block of memory to a new size.
func @free(ptr: *void);                                  // Frees a previously allocated block of memory.
func @memset(dst: *void, value: u8, size: u64);          // Sets a block of memory to a specific value.
func @memcpy(dst: *void, src: const *void, size: u64);   // Copies a block of memory from one location to another.
func @memmove(dst: *void, src: const *void, size: u64);  // Moves a block of memory, handling overlapping areas.
func @memcmp(dst, src: const *void, size: u64) -> s32;   // Compares two blocks of memory.
func @strlen(value: const *u8) -> u64;                   // Returns the length of a null-terminated string.

/**
# Atomic Operations
Atomic operations provide thread-safe manipulation of variables in shared memory, ensuring data consistency
without the need for explicit locking mechanisms.
*/

func @atomadd(addr: *s8, value: s8) -> s8;
func @atomadd(addr: *s16, value: s16) -> s16;
func @atomadd(addr: *s32, value: s32) -> s32;
func @atomadd(addr: *s64, value: s64) -> s64;
func @atomadd(addr: *u8, value: u8) -> u8;
func @atomadd(addr: *u16, value: u16) -> u16;
func @atomadd(addr: *u32, value: u32) -> u32;
func @atomadd(addr: *u64, value: u64) -> u64;

func @atomand(addr: *s8, value: s8) -> s8;
func @atomand(addr: *s16, value: s16) -> s16;
func @atomand(addr: *s32, value: s32) -> s32;
func @atomand(addr: *s64, value: s64) -> s64;
func @atomand(addr: *u8, value: u8) -> u8;
func @atomand(addr: *u16, value: u16) -> u16;
func @atomand(addr: *u32, value: u32) -> u32;
func @atomand(addr: *u64, value: u64) -> u64;

func @atomor(addr: *s8, value: s8) -> s8;
func @atomor(addr: *s16, value: s16) -> s16;
func @atomor(addr: *s32, value: s32) -> s32;
func @atomor(addr: *s64, value: s64) -> s64;
func @atomor(addr: *u8, value: u8) -> u8;
func @atomor(addr: *u16, value: u16) -> u16;
func @atomor(addr: *u32, value: u32) -> u32;
func @atomor(addr: *u64, value: u64) -> u64;

func @atomxor(addr: *s8, value: s8) -> s8;
func @atomxor(addr: *s16, value: s16) -> s16;
func @atomxor(addr: *s32, value: s32) -> s32;
func @atomxor(addr: *s64, value: s64) -> s64;
func @atomxor(addr: *u8, value: u8) -> u8;
func @atomxor(addr: *u16, value: u16) -> u16;
func @atomxor(addr: *u32, value: u32) -> u32;
func @atomxor(addr: *u64, value: u64) -> u64;

func @atomxchg(addr: *s8, exchangeWith: s8) -> s8;
func @atomxchg(addr: *s16, exchangeWith: s16) -> s16;
func @atomxchg(addr: *s32, exchangeWith: s32) -> s32;
func @atomxchg(addr: *s64, exchangeWith: s64) -> s64;
func @atomxchg(addr: *u8, exchangeWith: u8) -> u8;
func @atomxchg(addr: *u16, exchangeWith: u16) -> u16;
func @atomxchg(addr: *u32, exchangeWith: u32) -> u32;
func @atomxchg(addr: *u64, exchangeWith: u64) -> u64;

func @atomcmpxchg(addr: *s8, compareTo, exchangeWith: s8) -> s8;
func @atomcmpxchg(addr: *s16, compareTo, exchangeWith: s16) -> s16;
func @atomcmpxchg(addr: *s32, compareTo, exchangeWith: s32) -> s32;
func @atomcmpxchg(addr: *s64, compareTo, exchangeWith: s64) -> s64;
func @atomcmpxchg(addr: *u8, compareTo, exchangeWith: u8) -> u8;
func @atomcmpxchg(addr: *u16, compareTo, exchangeWith: u16) -> u16;
func @atomcmpxchg(addr: *u32, compareTo, exchangeWith: u32) -> u32;
func @atomcmpxchg(addr: *u64, compareTo, exchangeWith: u64) -> u64;

/**
# Math Intrinsics
These intrinsics provide various mathematical operations, including trigonometric, logarithmic, and other common
functions, offering precise control over mathematical calculations in Swag programs.
*/

func @sqrt(value: f32) -> f32;            // Computes the square root of a 32-bit floating-point number.
func @sqrt(value: f64) -> f64;            // Computes the square root of a 64-bit floating-point number.

func @sin(value: f32) -> f32;             // Computes the sine of a 32-bit floating-point number.
func @sin(value: f64) -> f64;             // Computes the sine of a 64-bit floating-point number.
func @cos(value: f32) -> f32;             // Computes the cosine of a 32-bit floating-point number.
func @cos(value: f64) -> f64;             // Computes the cosine of a 64-bit floating-point number.
func @tan(value: f32) -> f32;             // Computes the tangent of a 32-bit floating-point number.
func @tan(value: f64) -> f64;             // Computes the tangent of a 64-bit floating-point number.

func @sinh(value: f32) -> f32;            // Computes the hyperbolic sine of a 32-bit floating-point number.
func @sinh(value: f64) -> f64;            // Computes the hyperbolic sine of a 64-bit floating-point number.
func @cosh(value: f32) -> f32;            // Computes the hyperbolic cosine of a 32-bit floating-point number.
func @cosh(value: f64) -> f64;            // Computes the hyperbolic cosine of a 64-bit floating-point number.
func @tanh(value: f32) -> f32;            // Computes the hyperbolic tangent of a 32-bit floating-point number.
func @tanh(value: f64) -> f64;            // Computes the hyperbolic tangent of a 64-bit floating-point number.

func @asin(value: f32) -> f32;            // Computes the arc sine of a 32-bit floating-point number.
func @asin(value: f64) -> f64;            // Computes the arc sine of a 64-bit floating-point number.
func @acos(value: f32) -> f32;            // Computes the arc cosine of a 32-bit floating-point number.
func @acos(value: f64) -> f64;            // Computes the arc cosine of a 64-bit floating-point number.
func @atan(value: f32) -> f32;            // Computes the arc tangent of a 32-bit floating-point number.
func @atan(value: f64) -> f64;            // Computes the arc tangent of a 64-bit floating-point number.

func @log(value: f32) -> f32;             // Computes the natural logarithm of a 32-bit floating-point number.
func @log(value: f64) -> f64;             // Computes the natural logarithm of a 64-bit floating-point number.
func @log2(value: f32) -> f32;            // Computes the base-2 logarithm of a 32-bit floating-point number.
func @log2(value: f64) -> f64;            // Computes the base-2 logarithm of a 64-bit floating-point number.
func @log10(value: f32) -> f32;           // Computes the base-10 logarithm of a 32-bit floating-point number.
func @log10(value: f64) -> f64;           // Computes the base-10 logarithm of a 64-bit floating-point number.

func @floor(value: f32) -> f32;           // Computes the floor of a 32-bit floating-point number.
func @floor(value: f64) -> f64;           // Computes the floor of a 64-bit floating-point number.
func @ceil(value: f32) -> f32;            // Computes the ceiling of a 32-bit floating-point number.
func @ceil(value: f64) -> f64;            // Computes the ceiling of a 64-bit floating-point number.
func @trunc(value: f32) -> f32;           // Truncates a 32-bit floating-point number to its integer part.
func @trunc(value: f64) -> f64;           // Truncates a 64-bit floating-point number to its integer part.
func @round(value: f32) -> f32;           // Rounds a 32-bit floating-point number to the nearest integer.
func @round(value: f64) -> f64;           // Rounds a 64-bit floating-point number to the nearest integer.

func @abs(value: s8) -> s8;               // Computes the absolute value of an 8-bit signed integer.
func @abs(value: s16) -> s16;             // Computes the absolute value of a 16-bit signed integer.
func @abs(value: s32) -> s32;             // Computes the absolute value of a 32-bit signed integer.
func @abs(value: s64) -> s64;             // Computes the absolute value of a 64-bit signed integer.
func @abs(value: f32) -> f32;             // Computes the absolute value of a 32-bit floating-point number.
func @abs(value: f64) -> f64;             // Computes the absolute value of a 64-bit floating-point number.

func @exp(value: f32) -> f32;             // Computes the exponential function of a 32-bit floating-point number.
func @exp(value: f64) -> f64;             // Computes the exponential function of a 64-bit floating-point number.
func @exp2(value: f32) -> f32;            // Computes 2 raised to the power of a 32-bit floating-point number.
func @exp2(value: f64) -> f64;            // Computes 2 raised to the power of a 64-bit floating-point number.
func @pow(value1, value2: f32) -> f32;    // Computes the power function for 32-bit floating-point numbers.
func @pow(value1, value2: f64) -> f64;    // Computes the power function for 64-bit floating-point numbers.

func @min(value1, value2: s8) -> s8;      // Returns the minimum of two 8-bit signed integers.
func @min(value1, value2: s16) -> s16;    // Returns the minimum of two 16-bit signed integers.
func @min(value1, value2: s32) -> s32;    // Returns the minimum of two 32-bit signed integers.
func @min(value1, value2: s64) -> s64;    // Returns the minimum of two 64-bit signed integers.
func @min(value1, value2: u8) -> u8;      // Returns the minimum of two 8-bit unsigned integers.
func @min(value1, value2: u16) -> u16;    // Returns the minimum of two 16-bit unsigned integers.
func @min(value1, value2: u32) -> u32;    // Returns the minimum of two 32-bit unsigned integers.
func @min(value1, value2: u64) -> u64;    // Returns the minimum of two 64-bit unsigned integers.
func @min(value1, value2: f32) -> f32;    // Returns the minimum of two 32-bit floating-point numbers.
func @min(value1, value2: f64) -> f64;    // Returns the minimum of two 64-bit floating-point numbers.

func @max(value1, value2: s8) -> s8;      // Returns the maximum of two 8-bit signed integers.
func @max(value1, value2: s16) -> s16;    // Returns the maximum of two 16-bit signed integers.
func @max(value1, value2: s32) -> s32;    // Returns the maximum of two 32-bit signed integers.
func @max(value1, value2: s64) -> s64;    // Returns the maximum of two 64-bit signed integers.
func @max(value1, value2: u8) -> u8;      // Returns the maximum of two 8-bit unsigned integers.
func @max(value1, value2: u16) -> u16;    // Returns the maximum of two 16-bit unsigned integers.
func @max(value1, value2: u32) -> u32;    // Returns the maximum of two 32-bit unsigned integers.
func @max(value1, value2: u64) -> u64;    // Returns the maximum of two 64-bit unsigned integers.
func @max(value1, value2: f32) -> f32;    // Returns the maximum of two 32-bit floating-point numbers.
func @max(value1, value2: f64) -> f64;    // Returns the maximum of two 64-bit floating-point numbers.

func @bitcountnz(value: u8) -> u8;        // Counts the number of set bits in an 8-bit unsigned integer.
func @bitcountnz(value: u16) -> u16;      // Counts the number of set bits in a 16-bit unsigned integer.
func @bitcountnz(value: u32) -> u32;      // Counts the number of set bits in a 32-bit unsigned integer.
func @bitcountnz(value: u64) -> u64;      // Counts the number of set bits in a 64-bit unsigned integer.
func @bitcounttz(value: u8) -> u8;        // Counts the number of trailing zeros in an 8-bit unsigned integer.
func @bitcounttz(value: u16) -> u16;      // Counts the number of trailing zeros in a 16-bit unsigned integer.
func @bitcounttz(value: u32) -> u32;      // Counts the number of trailing zeros in a 32-bit unsigned integer.
func @bitcounttz(value: u64) -> u64;      // Counts the number of trailing zeros in a 64-bit unsigned integer.
func @bitcountlz(value: u8) -> u8;        // Counts the number of leading zeros in an 8-bit unsigned integer.
func @bitcountlz(value: u16) -> u16;      // Counts the number of leading zeros in a 16-bit unsigned integer.
func @bitcountlz(value: u32) -> u32;      // Counts the number of leading zeros in a 32-bit unsigned integer.
func @bitcountlz(value: u64) -> u64;      // Counts the number of leading zeros in a 64-bit unsigned integer.

func @byteswap(value: u16) -> u16;        // Swaps the byte order of a 16-bit unsigned integer.
func @byteswap(value: u32) -> u32;        // Swaps the byte order of a 32-bit unsigned integer.
func @byteswap(value: u64) -> u64;        // Swaps the byte order of a 64-bit unsigned integer.

func @rol(value, num: u8) -> u8;          // Rotates an 8-bit unsigned integer left by a specified number of bits.
func @rol(value, num: u16) -> u16;        // Rotates a 16-bit unsigned integer left by a specified number of bits.
func @rol(value, num: u32) -> u32;        // Rotates a 32-bit unsigned integer left by a specified number of bits.
func @rol(value, num: u64) -> u64;        // Rotates a 64-bit unsigned integer left by a specified number of bits.

func @ror(value, num: u8) -> u8;          // Rotates an 8-bit unsigned integer right by a specified number of bits.
func @ror(value, num: u16) -> u16;        // Rotates a 16-bit unsigned integer right by a specified number of bits.
func @ror(value, num: u32) -> u32;        // Rotates a 32-bit unsigned integer right by a specified number of bits.
func @ror(value, num: u64) -> u64;        // Rotates a 64-bit unsigned integer right by a specified number of bits.

func @muladd(val1, val2, val3: f32) -> f32;     // Performs a multiply-add operation for 32-bit floating-point numbers.
func @muladd(val1, val2, val3: f64) -> f64;     // Performs a multiply-add operation for 64-bit floating-point numbers.
