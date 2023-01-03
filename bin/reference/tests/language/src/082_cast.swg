/**
Swag will panic if a bad cast is performed.

Attribute is #[Swag.Safety("cast", true)].
You can also add 'SafetyCast' to the 'buildCfg.safetyGuards' variable.
*/

/**
Swag will check that a dynamic cast to an 'any' variable is valid.
*/
#test
{
    var x: any = "1"
    y := cast(string) x     // This is valid, because this is the correct underlying type
    //z := cast(s32) x      // This is not valid, and will panic
    //@assert(z == 0)
}
