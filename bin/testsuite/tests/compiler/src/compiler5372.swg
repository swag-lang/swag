struct B {}

struct A
{
    x:     [4] s32
    y:     B
}

//#global skip
#test
{
    let a, b: A
    @assert(a == b)
    a.x[3] = 1
    @assert(a != b)
}
