/**
# Return Value Usage
Swag enforces that all function return values must be used.
If a function’s result is ignored, the compiler raises an error.
This prevents accidental omission of important results and ensures deliberate handling of all return values.
*/
#test
{
    func sum(x, y: s32) => x + y

    // Uncommenting the following line would cause a compile-time error:
    // sum(2, 3)

    // Use 'discard' to explicitly ignore the return value
    discard sum(2, 3)
}

/**
# `Swag.Discardable` Attribute
Marking a function with '#[Swag.Discardable]' allows its return value to be safely ignored.
Use this for utility functions whose results are optional.
*/
#test
{
    #[Swag.Discardable]
    func mul(x, y: s32)->s32 => x * y

    // Return value can be ignored without using 'discard'
    mul(2, 4)
}
