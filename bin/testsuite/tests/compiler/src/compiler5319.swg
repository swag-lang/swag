struct Y
{
}

impl Y
{
    mtd clear()
    {
        @assert(false)
    }
}

struct X
{
    using base: Y
}

var G = 0

impl X
{
    mtd opDrop()
    {
        //X.clear(self)
    }

    mtd clear()
    {
        G += 1
        //@print("clear")
    }
}

struct A
{
    x: X
}

impl A
{
    //#[Swag.PrintBc]
    mtd clear()
    {
        x.clear()
    }
}

#test
{
    var a: A
    a.clear()
    @assert(G == 1)
}