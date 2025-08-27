using Core.Math

struct P
{
    bvMin:     Vector2
    bvMax:     Vector2
}

impl P
{
    //#[Swag.PrintBc]//, Swag.Optimize("", false)]
    mtd toto()
    {
        me.bvMin.set(Swag.F32.Max, Swag.F32.Max)
        me.bvMax.set(-Swag.F32.Max, -Swag.F32.Max)
    }
}

#test
{
    var t: P
    t.toto()
    @assert(t.bvMin.x == Swag.F32.Max)
    @assert(t.bvMin.y == Swag.F32.Max)
    @assert(t.bvMax.x == -Swag.F32.Max)
    @assert(t.bvMax.y == -Swag.F32.Max)
}
