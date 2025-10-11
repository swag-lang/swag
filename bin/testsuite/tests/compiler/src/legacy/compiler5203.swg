//#global skip
struct A
{
    str:     string
    x:       s32 = 66
}

struct B
{
    using a:     A
    y:           bool
}

//#[Swag.PrintBc]
#test
{
    let b = B{"str", y: true}
    @assert(b.str == "str")
    @assert(b.y == true)
    @assert(b.x == 66)
}
