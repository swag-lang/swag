// Functions can have the same names as long as parameters are different
#[Swag.constexpr]
{
    func sum(x, y: s32) => x + y
    func sum(x, y, z: s32) => x + y + z
}

#assert sum(1, 2) == 3
#assert sum(1, 2, 3) == 6