/**
By default, you must always use the returned value of a function, otherwise the compiler will generate an error.
*/

#test
{
    func sum(x, y: s32) => x + y

    // This would generated an error, because the return value of 'sum' is not used
    // sum(2, 3)

    // To force the return value to be ignored, you can use 'discard' at the call site
    discard sum(2, 3)
}

/**
If a function authorizes the caller to not use its return value, because it's not that important, it can be marked with `Swag.Discardable`.
*/

#test
{
    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // This is fine to ignore the return value of 'mul' (even if strange)
    mul(2, 4)
}
