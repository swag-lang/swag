/**
Swag will panic if some math operations are invalid.

Attribute is #[Swag.Safety("math", true)].
You can also add 'SafetyMath' to the 'buildCfg.safetyGuards' variable.
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
