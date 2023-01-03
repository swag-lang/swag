/**
Swag will panic if some operators overflow.

Attribute is #[Swag.Safety("overflow", true)].
You can also add 'SafetyOverflow' to the 'buildCfg.safetyGuards' variable.

Operators that can overflow are : + - * << >> and their equivalent += -= *= <<= >>=
*/

#test
{
    x := 255'u8
    // x += 1      // This would overflow, and panic, because we lose informations
    // x <<= 1     // Same here, we lose some bits
}

/**
But if you know what your are doing, you can use a special version of those operators, which will not panic.
Add the ',safe' modifier after the operation. This will disable safety checks.
*/
#test
{
    x := 255'u8
    x +=,safe 1     // wrap around
    @assert(x == 0)

    y := 255'u8
    y >>=,safe 1
    @assert(y == 127)
}


/**
Check that a cast does not lose information.
*/
#test
{
    x := 255'u8
    //y := cast(s8) x   // This would lose information and panic, as 255 cannot be encoded in 's8'
    //@print(y)
}

#test
{
    x := -1's8
    //y := cast(u8) y   // This cast also is not possible, because 'x' is negative and 'y' is 'u8'
    //@print(y)
}

/**
You can disable these safety checks with the corresponding attribute.
*/
#[Swag.Safety("overflow", false)]
#test
{
    x := 255'u8
    x += 255    // 254
    x += 1      // 255
    x >>= 1     // 127
    @assert(x == 127)
}