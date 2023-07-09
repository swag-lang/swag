/**
Functions can have the same names as long as their parameters are different.
*/
#[Swag.ConstExpr]
{
    func sum(x, y: s32) => x + y
    func sum(x, y, z: s32) => x + y + z
}

#assert sum(1, 2) == 3
#assert sum(1, 2, 3) == 6

/**
Note that in Swag, there is no implicit cast for function parameters. So you must always specify the right type.
*/
#test
{
    func over(x, y: s32) => x + y
    func over(x, y: s64) => x * y

    // This would generate an error because it's ambiguous, as 1 and 2 are not strong types
    // var res0 = over(1, 2)

    var res0 = over(1's32, 2's32)
    @assert(res0 == 3)
    var res1 = over(1's64, 2's64)
    @assert(res1 == 2)
}
