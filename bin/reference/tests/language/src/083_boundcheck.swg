/**
Swag will panic if an index is out of range when dereferencing a sized value like an array, a slice, a string...

Attribute is #[Swag.Safety("boundcheck", true)].
You can also add 'SafetyBoundCheck' to the 'buildCfg.safetyGuards' variable.
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
