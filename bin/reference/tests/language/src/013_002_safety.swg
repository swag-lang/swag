/**
# 'Safety Checks in Swag'

Swag provides safety checks that can be enabled at different granularity
levels — module, function, or even individual instruction — via
'#[Swag.Safety]'.

These checks prevent common programming errors by triggering panics during
unsafe operations (overflows, invalid math, out-of-bounds access, etc.).

You can also configure safety checks globally from the build configuration
with 'buildCfg.safetyGuards'.
*/

/**
> NOTE:
> Swag offers four predefined build configurations: 'debug', 'fast-debug',
> 'fast-compile', and 'release'. Safety checks are enabled by default in
> 'debug' and 'fast-debug', and disabled in 'fast-compile' and 'release'
> for performance.
*/

/**
# 'Overflow Safety'
Example:
#[Swag.Safety(.All, .Overflow, true)]
When enabled, Swag panics on arithmetic overflow or when bits are lost
during integer conversions.

Operators that can overflow include '+ - * << >>' and compound forms
'+= -= *= <<= >>='.
*/
#test
{
    var x = 255'u8
    // x += 1  // Uncomment to see overflow panic
}

/**
## Disabling Overflow Safety with '#wrap'

Use '#wrap' on the operation if overflow is expected and should not panic.
*/
#test
{
    var x = 255'u8
    x += #wrap 1
    @assert(x == 0)
}

/**
## Global Overflow Safety Control

Disable overflow safety checks in a scope with '#[Swag.CanOverflow(true)]'.
*/
#[Swag.CanOverflow(true)]
#test
{
    var x = 255'u8
    x += 1
    @assert(x == 0)
}

/**
## Promoting Operations to Prevent Overflow

For 8/16-bit operations, use '#prom' to promote to 32-bit and avoid overflow.
*/
#test
{
    let x = 255'u8 + #prom 1
    @assert(x == 256)
    @assert(#typeof(x) == u32)
}

/**
# 'Information Loss During Casting'

Swag checks casts for potential information loss between integer types.
*/
#test
{
    let x1 = 255'u8

    // var y0 = cast(s8) x1  // Would panic: 255 not representable as s8

    let y1 = cast #wrap (s8) x1
    @assert(y1 == -1)

    let x2 = -1's8
    // var y2 = cast(u8) x2  // Would panic: negative to unsigned
    let y2 = cast #wrap (u8) x2
    @assert(y2 == 255)
}

/**
## Disabling Overflow Safety Globally

Same as above: '#[Swag.CanOverflow(true)]' allows overflowing operations.
*/
#[Swag.CanOverflow(true)]
#test
{
    var x = 255'u8
    x += 255 // -> 254 (wrap)
    x += 1 // -> 255
    x >>= 1 // -> 127
    @assert(x == 127)
}

/**
# 'Dynamic Cast Type Safety'
Example:
#[Swag.Safety(.All, .DynCast, true)]
Swag panics if a cast from 'any' to another type is invalid.
*/
#test
{
    let x: any = "1"
    let y  = cast(string) x
    // var z = cast(s32) x  // Would panic: underlying type mismatch
    // @assert(z == 0)
}

/**
Swag also panics if casting from an interface to a pointer-to-struct
cannot be performed.
*/

/**
# 'Array Bounds Checking'
Example:
#[Swag.Safety(.All, .BoundCheck, true)]
Swag panics if an index is out of range when dereferencing sized values
(arrays, slices, strings).
*/
#test
{
    var x   = [0, 1, 2]
    var idx = 10
    // @assert(x[idx] == 1)  // Would panic: out-of-bounds
}

/**
## Safety When Indexing a Slice

Indexing a slice is checked for bounds.
*/
#test
{
    let x:  const [..] s32 = [0, 1, 2]
    var idx = 1
    @assert(x[idx] == 1)
    idx += 9
    // @assert(x[idx] == 1)  // Would panic: out-of-bounds
}

/**
## Safety When Slicing a Sized Value

Slice operations are checked for bounds.
*/
#test
{
    var x: const [..] s32 = [0, 1, 2]
    // var slice = x[1..4]   // Would panic: out-of-bounds
    // @assert(slice[0] == 1)
}

#test
{
    var x   = "string"
    var idx = 10
    // var slice = x[0..idx] // Would panic: out-of-bounds
    // @assert(slice[0] == 's')
}

/**
# 'Math Safety'
Example:
#[Swag.Safety(.All, .Math, true)]
Swag panics for invalid math, such as division by zero or invalid intrinsic
arguments.
*/
#test
{
    var x = 1'f32
    var y = 0'f32
    // var z = x / y  // Would panic: division by zero
    // @print(z)
}

/**
## Checking Invalid Math Intrinsic Arguments

Swag validates arguments for several math intrinsics and panics if invalid.
*/
#test
{
    // @abs(-128)        // Invalid for abs on this target
    // @log(-2'f32)      // Invalid: log of negative
    // @log2(-2'f32)     // Invalid: log2 of negative
    // @log10(2'f64)     // Example: implementation-specific constraints
    // @sqrt(-2'f32)     // Invalid: sqrt of negative
    // @asin(-2'f32)     // Invalid: asin out of range
    // @acos(2'f32)      // Invalid: acos out of range
}

/**
# 'Switch Safety'
Example:
#[Swag.Safety(.All, .Switch, true)]
With '#[Swag.Complete]', Swag panics if a switch does not cover all cases.
*/
#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    func colorToString(color: Color)->string
    {
        // #[Swag.Complete]
        switch color
        {
        case Color.Red:
            return "Red"
        case Color.Green:
            return "Green"
        }

        return ""
    }
}

/**
# 'Boolean Safety'
Example:
#[Swag.Safety(.All, .Bool, true)]
Swag panics if a 'bool' is not 'true' (1) or 'false' (0).
*/
#test
{
    var b: u8 = 2
    // if b { }  // Would panic: invalid boolean value
}

/**
# 'NaN Safety'
Example:
#[Swag.Safety(.All, .NaN, true)]
Swag panics if a floating-point 'NaN' participates in an operation,
preventing propagation of invalid values.
*/
