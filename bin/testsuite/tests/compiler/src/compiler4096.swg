struct Arr
{
    tt:  s32 = 5
}

//#[Swag.PrintBc]
struct T
{
    x:     Arr
    y:     [2] Arr
}

#test
{
    var t: T
    //@print(t.x.tt, " ", t.y[0].tt, " ", t.y[1].tt)
    @assert(t.x.tt == 5)
    @assert(t.y[0].tt == 5)
    @assert(t.y[1].tt == 5)
}
