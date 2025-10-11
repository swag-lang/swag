/**
# Safety Checks in Swag

Swag provides various safety checks that can be enabled at different granularity levels—module,
function, or even individual instruction—using the `#[Swag.Safety]` attribute.

These safety checks are designed to prevent common programming errors by triggering panics during
unsafe operations, such as overflows, invalid math operations, or out-of-bounds access.

You can also configure safety checks globally based on the build configuration using
`buildCfg.safetyGuards`.

> NOTE:
> Swag offers four predefined build configurations: `debug`, `fast-debug`, `fast-compile`, and `release`. Safety checks are enabled by default in `debug` and `fast-debug`, but they are disabled in `fast-compile` and `release` for performance reasons.
*/

/**
# Overflow Safety
```swag
#[Swag.Safety("overflow", true)]
```
When overflow safety is enabled, Swag will panic if arithmetic operations overflow or if bits
are lost during an integer conversion.

Operators that can cause overflows include: `+ - * << >>` and their compound assignments `+= -= *=
<<= >>=`.
*/
#test
{
    var x = 255'u8 // Initialize x with the maximum value for u8
    // x += 1                                      // Uncommenting this will cause a panic due to overflow
}

/**
## Disabling Overflow Safety with `#wrap`

If an overflow is expected and should not cause a panic, the `#wrap` modifier can be used with the
operation to bypass the safety check.
*/
#test
{
    var x = 255'u8 // Initialize x with the maximum value for u8
    x += #wrap 1 // This will wrap around without causing a panic
    @assert(x == 0) // Assert that x has wrapped around to 0
}

/**
## Global Overflow Safety Control

To disable overflow safety checks globally within a scope, use `#[Swag.CanOverflow(true)]`. This
prevents overflows from causing panics for all operations in the scope.
*/
#[Swag.CanOverflow(true)]
#test
{
    var x = 255'u8 // Initialize x with the maximum value for u8
    x += 1 // Overflow occurs, but no panic due to global setting
    @assert(x == 0) // Assert that x has wrapped around to 0
}

/**
## Promoting Operations to Prevent Overflow

For operations involving 8-bit or 16-bit integers, you can use the `#prom` modifier to promote the
operation to 32-bit, thereby avoiding overflow by widening the operand types.
*/
#test
{
    let x = 255'u8 + #prom 1 // Promote the addition to 32-bit to avoid overflow
    @assert(x == 256) // Assert that the result is 256
    @assert(#typeof(x) == u32) // Assert that the type of x is u32
}

/**
# Information Loss During Casting

Swag checks for potential information loss during type casting operations, such as when converting
between different integer types.
*/
#test
{
    let x1 = 255'u8 // Initialize x1 with the maximum value for u8

    // var y0 = cast(s8) x1                       // This would cause a panic because 255 cannot be
    // represented as s8

    let y1 = cast #wrap (s8) x1 // Use #wrap to bypass safety checks and allow this
    // cast
    @assert(y1 == -1) // Assert that y1 is -1 after wrapping

    let x2 = -1's8 // Initialize x2 with the minimum value for s8
    // var y2 = cast(u8) x2                       // This would cause a panic because x2 is negative
    let y2 = cast #wrap (u8) x2 // Use #wrap to bypass safety checks
    @assert(y2 == 255) // Assert that y2 is 255 after wrapping
}

/**
## Disabling Overflow Safety Globally

Safety checks for overflow can be globally disabled, allowing operations that would typically
panic due to overflow to proceed normally.
*/
#[Swag.CanOverflow(true)]
#test
{
    var x = 255'u8 // Initialize x with the maximum value for u8
    x += 255 // x becomes 254 after wrapping
    x += 1 // x becomes 255
    x >>= 1 // x becomes 127 after right shift
    @assert(x == 127) // Assert that x is 127
}

/**
# Dynamic Cast Type Safety
```swag
#[Swag.Safety("dyncast", true)]
```
Swag will panic if a cast from the `any` type to another type is invalid, ensuring type safety
when working with dynamic types.
*/
#test
{
    let x: any = "1"            // Initialize x with a string
    let y  = cast(string) x     // This is valid because the underlying type is correct
    // var z = cast(s32) x                        // This is invalid and will cause a panic
    // @assert(z == 0)
}

/**
Swag will also panic if casting from an interface to a pointer to struct that cannot be performed.
*/

/**
# Array Bounds Checking
```swag
#[Swag.Safety("boundcheck", true)]
```
Swag will panic if an index is out of range when dereferencing a sized value such as an array, a
slice, or a string.
*/
#test
{
    var x   = [0, 1, 2]     // Initialize an array
    var idx = 10            // Set an out-of-bounds index
    // @assert(x[idx] == 1)                       // This will cause a panic due to out-of-bounds access
}

/**
## Safety When Indexing a Slice

Swag ensures that indexing operations on slices are within bounds to prevent runtime errors.
*/
#test
{
    let x:  const [..] s32 = [0, 1, 2]     // Initialize a slice
    var idx = 1                            // Set a valid index
    @assert(x[idx] == 1) // Assert that x[1] is 1
    idx += 9 // Move the index out of bounds
    // @assert(x[idx] == 1)                       // This will cause a panic due to out-of-bounds access
}

/**
## Safety When Slicing a Sized Value

Swag will panic if a slice operation goes out of bounds, ensuring safe slicing of arrays or strings.
*/
#test
{
    var x: const [..] s32 = [0, 1, 2] // Initialize a slice
    // var slice = x[1..4]                        // This will cause a panic due to out-of-bounds access
    // @assert(slice[0] == 1)
}

#test
{
    var x   = "string"     // Initialize a string
    var idx = 10           // Set an out-of-bounds index for slicing
    // var slice = x[0..idx]                      // This will cause a panic due to out-of-bounds slicing
    // @assert(slice[0] == 's')
}

/**
# Math Safety
```swag
#[Swag.Safety("math", true)]
```
Swag will panic if certain math operations are invalid, such as division by zero or invalid
arguments to math functions.
*/
#test
{
    var x = 1'f32     // Initialize x with a float value
    var y = 0'f32     // Initialize y with zero
    // var z = x / y                              // Division by zero will cause a panic
    // @print(z)
}

/**
## Checking Invalid Math Intrinsic Arguments

Swag also checks for invalid arguments passed to certain math intrinsics, causing a panic if the
arguments are unsupported or invalid.
*/
#test
{
    // The following operations will panic if the arguments are invalid:

    // @abs(-128)                                 // Invalid argument for abs
    // @log(-2'f32)                               // Logarithm of a negative number is invalid
    // @log2(-2'f32)                              // Logarithm base 2 of a negative number is invalid
    // @log10(2'f64)                              // Logarithm base 10 of a negative number is invalid
    // @sqrt(-2'f32)                              // Square root of a negative number is invalid
    // @asin(-2'f32)                              // Arc sine out of valid range is invalid
    // @acos(2'f32)                               // Arc cosine out of valid range is invalid
}

/**
# Switch Safety
```swag
#[Swag.Safety("switch", true)]
```
Swag will panic if a switch statement marked with `#[Swag.Complete]` does not cover all possible
cases, ensuring exhaustive pattern matching.
*/
#test
{
    enum Color
    {
        Red
        Green
        Blue
        // Define an enum with three values
    }

    func colorToString(color: Color)->string
    {
        // #[Swag.Complete]                       // Mark the switch as complete
        switch color
        {
        case Color.Red:
            return "Red" // Handle Red
        case Color.Green:
            return "Green" // Handle Green
        }

        return "" // Return an empty string as a fallback
    }
}

/**
# Boolean Safety
```swag
#[Swag.Safety("bool", true)]
```
Swag will panic if a boolean value is not either `true` (1) or `false` (0), enforcing strict
boolean type safety.
*/
#test
{
    var b: u8 = 2 // Initialize b with an invalid boolean value
    // if b { ... }                               // This will panic because b is not a valid boolean
}

/**
# NaN Safety
```swag
#[Swag.Safety("nan", true)]
```
Swag will panic if a floating-point `NaN` (Not a Number) is used in an operation, ensuring that
NaNs do not propagate through calculations.
*/
