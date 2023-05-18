/**
Swag comes with a bunch of safety checks which can be activated by module/function/instruction with the `#[Swag.Safety]` attribute.

Safety checks can also be changed for a specific build configuration (`--cfg:<config>`) with `buildCfg.safetyGuards`.

> Swag comes with four predefined configurations : `debug`, `fast-debug`, `fast-compile` and `release`. Safety checks are disabled in `fast-compile` and `release`.
*/

/**
# #[Swag.Safety("overflow", true)]
Swag will panic if some operators overflow and if we lose some bits during an integer conversion.

Operators that can overflow are : `+ - * << >>` and their equivalent `+= -= *= <<= >>=`.
*/

#test
{
    x := 255'u8
    // x += 1      // This would overflow, and panic, because we lose informations
}

/**
But if you know what your are doing, you can use a special version of those operators, which will not panic. Add the `,over` modifier after the operation. This will disable safety checks.
*/
#test
{
    x := 255'u8
    x +=,over 1     // Overflow is expected, so this will wrap around
    @assert(x == 0)
}

/**
You can also use `#[Swag.Overflow(true)]` to authorize overflow on a more global scale.
*/
#[Swag.Overflow(true)]
#test
{
    x := 255'u8
    x += 1     // No need for operator modifier ',over'
    @assert(x == 0)
}

/**
For 8 or 16 bits, you can promote an operation to 32 bits by using ',up'.
*/
#test
{
    x := 255'u8 +,up 1
    @assert(x == 256)
    @assert(@typeof(x) == u32)
}

/**
Swag will also check that a cast does not lose information.
*/
#test
{
    x1 := 255'u8

    //y0 := cast(s8) x1     // This would lose information and panic, as 255 cannot be encoded in 's8'
    //@print(y0)

    y1 := cast,over(s8) x1  // But ',over' can be used on the cast operation too
    @assert(y1 == -1)

    x2 := -1's8
    //y2 := cast(u8) x2     // This cast also is not possible, because 'x2' is negative and 'y' is 'u8'
    //@print(y)
    y2 := cast,over(u8) x2
    @assert(y2 == 255)
}

/**
Rember that you can disable these safety checks with the corresponding attribute.
*/
#[Swag.Overflow(true)]
#test
{
    x := 255'u8
    x += 255    // 254
    x += 1      // 255
    x >>= 1     // 127
    @assert(x == 127)
}

/**
# #[Swag.Safety("any", true)]
Swag will panic if a bad cast from 'any' is performed.
*/

#test
{
    var x: any = "1"
    y := cast(string) x     // This is valid, because this is the correct underlying type
    //z := cast(s32) x      // This is not valid, and will panic
    //@assert(z == 0)
}

/**
# #[Swag.Safety("boundcheck", true)]
Swag will panic if an index is out of range when dereferencing a sized value like an array, a slice, a string...
*/

/**
Safety for fixed size arrays.
*/
#test
{
    x := [0, 1, 2]
    idx := 10
    //@assert(x[idx] == 1)     // '10' is out of range, will panic
}

/**
Safety when indexing a slice.
*/
#test
{
    var x: const [..] s32 = [0, 1, 2]
    idx := 1
    @assert(x[idx] == 1)        // '1' is in range, ok
    idx += 9
    //@assert(x[idx] == 1)      // '10' is out of range, will panic
}

/**
Safety when slicing a sized value.
*/
#test
{
    var x: const [..] s32 = [0, 1, 2]
    //slice := x[1..4]              // '4' is out of range, will panic
    //@assert(slice[0] == 1)
}

#test
{
    x := "string"
    idx := 10
    //slice := x[0..idx]            // '10' is out of range, will panic
    //@assert(slice[0] == "s"'u8)
}

/**
# #[Swag.Safety("math", true)]
Swag will panic if some math operations are invalid.
*/

/**
Division by zero.
*/
#test
{
    x := 1'f32
    y := 0'f32
    //z := x / y        // Division by zero, panic
    //@print(z)
}

/**
Swag will also check for invalid arguments on some math intrinsics.
*/
#test
{
    // All of this will panic, because arguments are incorrect.

    //@abs(-128)
    //@log(-2'f32)
    //@log2(-2'f32)
    //@log10(2'f64)
    //@sqrt(-2'f32)
    //@asin(-2'f32)
    //@acos(2'f32)
}

/**
# #[Swag.Safety("switch", true)]
Swag will panic if a switch is marked with #[Swag.Complete], but the value is not covered by a 'case'.
*/

/**
# #[Swag.Safety("bool", true)]
Swag will panic if a boolean value is not 'true' (1) or 'false' (0).
*/

/**
# #[Swag.Safety("nan", true)]
Swag will panic if a floating point 'NaN' is used.
*/