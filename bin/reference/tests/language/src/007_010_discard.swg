/**
# Return Value Usage
In Swag, there is a strict requirement that every function's return value must be utilized. This design choice
helps prevent potential bugs that can arise from accidentally ignoring the result of a function call. If a function
is invoked and its return value is not used, the compiler will generate an error. This ensures that developers are
consciously handling all return values, which can be critical for maintaining the correctness of the code.
*/

#test
{
    func sum(x, y: s32) => x + y

    // Uncommenting the following line would generate a compiler error,
    // because the return value of 'sum' is not used.
    // sum(2, 3)

    // To explicitly discard the return value when you don't need it, use 'discard' at the call site.
    // The return value of 'sum' is intentionally ignored here.
    discard sum(2, 3)
}

/**
# `Swag.Discardable` Attribute
There are scenarios where the return value of a function may be optional or non-critical to the operation.
In such cases, the function can be marked with the `Swag.Discardable` attribute. This attribute permits the caller
to ignore the return value without triggering a compiler error, making it clear that the result is not necessarily
intended to be used.
*/

#test
{
    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // It's allowed to ignore the return value of 'mul' without using 'discard'.
    mul(2, 4)
}
