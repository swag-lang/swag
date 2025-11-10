struct Arr
{
    tt:  s32
}

var G = 0

impl Arr
{
    mtd opDrop()
    {
        G += me.tt
    }
}

//#[Swag.PrintBc]
struct T
{
    x:     Arr
    y:     [2] Arr
}

#test
{
    {
        var t: T
        t.x.tt    = 1
        t.y[0].tt = 2
        t.y[1].tt = 3
    }

    @assert(G == 6)
}
