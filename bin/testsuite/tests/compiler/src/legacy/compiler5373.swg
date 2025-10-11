struct B {}

struct A
{
    x:     [4] s32
    y:     [4] B
}

//#global skip
#test
{
    let a, b: A
    @assert(a == b)
}
