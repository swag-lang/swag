/**
Swag will panic if some values are not in the correct 'range'.

Attribute is #[Swag.Safety("range", true)].
You can also add 'SafetyRange' to the 'buildCfg.safetyGuards' variable.
*/

/**
Panic if a dynamic switch is marked with #[Swag.Complete], but the value is not covered by a 'case'.
*/
#test
{
    a := 42'u8

    // This would panic because the value of 'a' (42) is not covered by a 'case'.
    #[Swag.Safety("range", false)]
    #[Swag.Complete]
    switch a
    {
    case 1: break
    case 2: break
    }
}
