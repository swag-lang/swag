struct Arr
{
    tt: s32 = 5
}

var G = 0

impl Arr
{
    mtd opPostMove()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
struct T
{
    x: Arr
    y: [2] Arr
}

#test
{
    var t1, t2: T
    t1 = #move t2
    @assert(G == 3)
    t1 = t2
    G = 0
    @assert(G == 0)
}