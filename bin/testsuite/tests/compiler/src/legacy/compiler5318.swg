struct(T) Y {}

impl Y
{
    mtd clear()
    {
        @assert(false)
    }
}

struct(T) X
{
    using base: Y
}

var G = 0

impl X
{
    mtd opDrop()
    {
        //X.clear(me)
    }

    mtd clear()
    {
        G += 1
        //@print("clear")
    }
}

struct A
{
    x:   X's32
}

impl A
{
    //#[Swag.PrintBc]
    mtd clear()
    {
        me.x.clear()
    }
}

#test
{
    var a: A
    a.clear()
    @assert(G == 1)
}
