#[Swag.EnumIndex]
enum A { X, Y, Z }

#test
{
    let x = [1, 2, 3]
    let y = x[A.Z]
    @assert(y == 3)
}
