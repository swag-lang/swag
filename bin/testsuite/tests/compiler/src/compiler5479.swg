//#global skip
struct AA
{
    x    = 0
}

impl AA
{
    func opDrop(me)
    {
        //@print("drop")
    }

    func opPostMove(me)
    {
        //@print("move")
    }

    func from()->AA
    {
        return {666}
    }
}

//#[Swag.PrintBc]//, Swag.Optimize(.All, false)]
//#[Swag.Inline]
func toto()->{ x: AA }
{
    return {AA.from()}
}

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(x.x.x == 666)
}
