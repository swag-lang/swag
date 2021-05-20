#[Swag.pack(0)]
struct Union
{
    x: string
    a, b, c, d, e: s32
}

struct B
{
    v: Union
    w: s32
}

#test
{
    @assert(@sizeof(B) == 24)
}