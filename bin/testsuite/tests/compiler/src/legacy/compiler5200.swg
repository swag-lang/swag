//#global skip
struct A
{
    str:     string
    x:       s32
}

struct B
{
    using a:     A
    y:           bool
}

#test
{
    let b = B{"str", 0, true}
    @assert(b.str == "str")
}
