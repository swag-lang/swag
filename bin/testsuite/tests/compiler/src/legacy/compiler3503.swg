var G = 0

struct A {}

struct B
{
    using a: A
}

impl A
{
    mtd clear()
    {
        G += 2
    }
}

impl B
{
    mtd clear()
    {
        G = 1
        me.a.clear()
    }
}

#test
{
    var b: B
    b.clear()
    @assert(G == 3)
}
