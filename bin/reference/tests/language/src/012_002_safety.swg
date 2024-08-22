/**
# Safety Checks in Swag

Swag provides a variety of safety checks that can be enabled at different levels—module, function, or even individual instruction—using the `#[Swag.Safety]` attribute.

These safety checks are designed to prevent common programming errors by triggering panics during unsafe operations, such as overflows, invalid math operations, or out-of-bounds access.

You can also configure safety checks globally based on the build configuration using `buildCfg.safetyGuards`.

> Note: Swag offers four predefined build configurations: `debug`, `fast-debug`, `fast-compile`, and `release`. Safety checks are enabled by default in `debug` and `fast-debug`, but they are disabled in `fast-compile` and `release` for performance reasons.
*/

/**
# Overflow Safety
```swag
#[Swag.Safety("overflow", true)]
```
When overflow safety is enabled, Swag will panic if arithmetic operations overflow or if bits are lost during an integer conversion.

Operators that can cause overflows include: `+ - * << >>` and their compound assignments `+= -= *= <<= >>=`.
*/
#test
{
    var x = 255'u8
    // x += 1  // Uncommenting this will cause a panic because it overflows
}

/**
## Disabling Overflow Safety with `#over`

If you are certain that an overflow is expected and you want to prevent a panic, you can use the `#over` modifier with the operation. This will bypass the safety check.
*/
#test
{
    var x = 255'u8
    x += #over 1 // This will wrap around without causing a panic
    @assert(x == 0)
}

/**
## Global Overflow Safety Control

You can globally allow overflows by using `#[Swag.Overflow(true)]`, which disables the overflow safety checks for all operations within the scope.
*/
#[Swag.Overflow(true)]
#test
{
    var x = 255'u8
    x += 1 // No need for `#over` since overflow is globally allowed
    @assert(x == 0)
}

/**
## Promoting Operations to Prevent Overflow

For 8-bit or 16-bit integers, you can promote an operation to 32-bit using the `#prom` modifier. This avoids overflow by widening the operand types.
*/
#test
{
    let x = 255'u8 + #prom 1
    @assert(x == 256)
    @assert(@typeof(x) == u32)
}

/**
# Information Loss During Casting

Swag will also check for potential information loss during type casting operations, such as converting between different integer types.
*/
#test
{
    let x1 = 255'u8

    // var y0 = cast(s8) x1     // This would cause a panic because 255 cannot be represented as `s8`
    // @print(y0)

    let y1 = cast(s8) #over x1 // Use `#over` to bypass safety checks and allow this cast
    @assert(y1 == -1)

    let x2 = -1's8
    // var y2 = cast(u8) x2     // This would also cause a panic because `x2` is negative
    let y2 = cast(u8) #over x2
    @assert(y2 == 255)
}

/**
## Disabling Overflow Safety Globally

Here, safety checks for overflow are disabled, allowing operations that would typically panic due to overflow to proceed normally.
*/
#[Swag.Overflow(true)]
#test
{
    var x = 255'u8
    x += 255 // x becomes 254
    x += 1   // x becomes 255
    x >>= 1  // x becomes 127
    @assert(x == 127)
}

/**
# `any` Type Safety
```swag
#[Swag.Safety("any", true)]
```
Swag will panic if a cast from the `any` type to another type is invalid.
*/
#test
{
    let x: any = "1"
    let y  = cast(string) x     // This is valid because the underlying type is correct
    // var z = cast(s32) x      // This is invalid and will cause a panic
    // @assert(z == 0)
}

/**
# Array Bounds Checking
```swag
#[Swag.Safety("boundcheck", true)]
```
Swag will panic if an index is out of range when dereferencing a sized value such as an array, a slice, or a string.
*/
#test
{
    var x   = [0, 1, 2]
    var idx = 10
    // @assert(x[idx] == 1)     // `10` is out of range, this will cause a panic
}

/**
## Safety When Indexing a Slice

Swag ensures that indexing operations on slices are within bounds.
*/
#test
{
    let x: const [..] s32 = [0, 1, 2]
    var idx = 1
    @assert(x[idx] == 1) // `1` is in range, so this is safe
    idx += 9
    // @assert(x[idx] == 1)      // `10` is out of range, this will cause a panic
}

/**
## Safety When Slicing a Sized Value

Swag will panic if a slice operation goes out of bounds.
*/
#test
{
    var x: const [..] s32 = [0, 1, 2]
    // var slice = x[1..4]      // `4` is out of range, this will cause a panic
    // @assert(slice[0] == 1)
}

#test
{
    var x   = "string"
    var idx = 10
    // var slice = x[0..idx]    // `10` is out of range, this will cause a panic
    // @assert(slice[0] == `s`)
}

/**
# Math Safety
```swag
#[Swag.Safety("math", true)]
```
Swag will panic if certain math operations are invalid, such as division by zero.
*/
#test
{
    var x = 1'f32
    var y = 0'f32
    // var z = x / y        // Division by zero will cause a panic
    // @print(z)
}

/**
## Checking Invalid Math Intrinsic Arguments

Swag also checks for invalid arguments passed to certain math intrinsics, which will cause a panic if unsupported.
*/
#test
{
    // All of the following operations will panic if the arguments are invalid:

    // @abs(-128)
    // @log(-2'f32)
    // @log2(-2'f32)
    // @log10(2'f64)
    // @sqrt(-2'f32)
    // @asin(-2'f32)
    // @acos(2'f32)
}

/**
# Switch Safety
```swag
#[Swag.Safety("switch", true)]
```
Swag will panic if a switch statement marked with `#[Swag.Complete]` does not cover all possible cases.
*/
#test
{
    enum Color { Red, Green, Blue }

    func colorToString(color: Color) -> string
    {
        // #[Swag.Complete]
        switch color
        {
            case Color.Red: return "Red"
            case Color.Green: return "Green"
            // If `Color.Blue` is not covered, this will cause a panic
        }

        return ""
    }
}

/**
# Boolean Safety
```swag
#[Swag.Safety("bool", true)]
```
Swag will panic if a boolean value is not either `true` (1) or `false` (0).
*/
#test
{
    var b: u8 = 2
    // if b { ... }    // This will panic because `b` is not a valid boolean
}

/**
# NaN Safety
```swag
#[Swag.Safety("nan", true)]
```
Swag will panic if a floating-point `NaN` (Not a Number) is used in an operation.
*/
