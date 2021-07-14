#[Swag.EnumIndex]
enum A { X; Y; Z; }

#test
{
    x := @[1, 2, 3]
    y := x[A.Z]
    @assert(y == 3)
}
