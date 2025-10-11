var cpt: s32 = 0

struct Point {}

//#[Swag.PrintBc]
func(T) toto()
{
    let t = #typeof(T)
    @assert(t.fullname == "__compiler793.Point")
    if (t == #typeof(Point)) do
        cpt = 1
}

//#[Swag.PrintBc]
#test
{
    cpt = 0
    toto'(Point)()
    @assert(cpt == 1)
}
