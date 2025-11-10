alias Point       = s64
alias Triangle    = s64
alias Orientation = bool

struct A {}
impl A
{
    mtd orient2d(pa, pb, pc: Point)->Orientation
    {
        return true
    }

    mtd nextFlipTriangle(o: Orientation, t, ot: Triangle, p, op: Point)->Triangle
    {
        return 1
    }

    var cpt = 0

    //#[Swag.PrintBc, Swag.Optimize(.All, true)]
    mtd flipEdgeEvent(ep, eq: Point, tri: Triangle, p: Point)
    {
        //@print(ep, " ", eq, " ", tri, " ", p, "\n")
        @assert(tri == 1)
        if cpt == 2 do
            return
        cpt += 1
        var t  = tri
        let ot = t
        let op = t

        let o = me.orient2d(eq, op, ep)
        t = me.nextFlipTriangle(o, t, ot, p, op)
        me.flipEdgeEvent(ep, eq, t, p)
    }
}

#test
{
    var x: A
    x.flipEdgeEvent(1, 1, 1, 1)
}
