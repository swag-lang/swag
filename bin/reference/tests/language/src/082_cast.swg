/**
Swag will panic if a bad cast is performed.

Attribute is #[Swag.Safety("cast", true)].
You can also add 'SafetyCA' to the 'buildCfg.safetyGuards' variable.
*/

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
Swag will also check that a dynamic cast to an 'any' variable is valid.
*/
#test
{
    var x: any = "1"
    y := cast(string) x     // This is valid, because this is the correct underlying type
    //z := cast(s32) x      // This is not valid, and will panic
    //@assert(z == 0)
}

/**
You can disable these safety checks with the corresponding attribute.
*/
#[Swag.Safety("cast", false)]
#test
{
    x := 255'u8
    y := cast(s8) x
    @assert(y == -1)

    z := 0xFFFF'u16
    w := cast(u8) x
    @assert(w == 0xFF)
}