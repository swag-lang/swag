#[Swag.EnumIndex]
enum A { X; Y; Z; }

#test
{
    var x = [1, 2, 3]
    var y = x[A.Z]
    @assert(y == 3)
}
