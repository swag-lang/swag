//#global skip
struct AA
{
    x = 0
}

impl AA
{
    func opDrop(self)
    {
        //@print("drop")
    }

    func opPostMove(self)
    {
        //@print("move")
    }

    func from()->AA
    {
        return {666}
    }
}

//#[Swag.PrintBc]//, Swag.Optimize("", false)]
#[Swag.Inline]
func toto()->{x: AA}
{
    return {AA.from()}
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
    @assert(x.x.x == 666)
}