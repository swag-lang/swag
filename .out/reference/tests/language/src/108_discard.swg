#test
{
    // By default, you must always use the returned value of a function,
    // otherwise the compiler will generate an error
    func sum(x, y: s32) => x + y

    // This would generated an error, because the return value of 'sum' is not used
    //sum(2, 3)

    // To force the return value to be ignored, you can use 'discard'
    discard sum(2, 3)

    // If a function authorizes the caller to not use its return value, because
    // it's not that important, it can be marked with 'Swag.Discardable'

    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // This is fine to ignore the return value of 'mul'
    mul(2, 4)
}