/**
Functions can have the same names as long as their parameters are different. Such function must be marked with '#[Swag.Overload]'.
*/
#[Swag.ConstExpr, Swag.Overload]
{
    func sum(x, y: s32)    => x + y
    func sum(x, y, z: s32) => x + y + z
}

#assert sum(1, 2) == 3
#assert sum(1, 2, 3) == 6

/**
Note that in Swag, there is no implicit cast for function parameters in case of overloads. So you must always specify the right type, in order to be sure to call the right version.
*/
#test
{
    #[Swag.Overload]
    func over(x, y: s32) => x + y
    #[Swag.Overload]
    func over(x, y: s64) => x * y

    // This would generate an error because it's ambiguous, as 1 and 2 are not strong types
    // var res0 = over(1, 2)

    let res0 = over(1's32, 2's32)
    @assert(res0 == 3)
    let res1 = over(1's64, 2's64)
    @assert(res1 == 2)
}
