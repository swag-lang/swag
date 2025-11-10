struct A
{
    x:   s32 = undefined
}

//#[Swag.PrintBc]
struct B { x: A{1} }

impl B
{
    mtd opDrop() {}
}

#test
{
    var b: B
    @assert(b.x.x == 1)
}
