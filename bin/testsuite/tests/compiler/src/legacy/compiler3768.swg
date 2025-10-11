struct(T) A {}

impl A
{
    mtd clear()
    {
        @assert(false)
    }
}

struct(T) B
{
    using a: A'(*T)
}

var G = 0
impl B
{
    mtd clear()
    {
        G = 1
    }

    mtd toto()
    {
        me.clear()
    }
}

#test
{
    var b: B's32
    b.toto()
    @assert(G == 1)
}
