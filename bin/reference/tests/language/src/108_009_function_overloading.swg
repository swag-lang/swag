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