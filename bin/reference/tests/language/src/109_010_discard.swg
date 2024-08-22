/**
# Return Value Usage
In Swag, you must always use the returned value of a function by default. If you call a function and do not use its return value, the compiler will generate an error. This strict enforcement helps catch potential bugs where a return value is unintentionally ignored.
*/

#test
{
    func sum(x, y: s32) => x + y

    // Uncommenting the following line would generate a compiler error,
    // because the return value of 'sum' is not used.
    // sum(2, 3)

    // To explicitly discard the return value when you don't need it, use 'discard' at the call site.
    discard sum(2, 3) // The return value of 'sum' is intentionally ignored here.
}

/**
# `Swag.Discardable` Attribute
If the function authorizes the caller to ignore its return value, the function can be marked with `Swag.Discardable`. This is useful for functions where the return value is optional and not critical to the operation.
*/

#test
{
    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // It's allowed to ignore the return value of 'mul' without using 'discard'.
    mul(2, 4) // No error, even though the return value is not used.
}
