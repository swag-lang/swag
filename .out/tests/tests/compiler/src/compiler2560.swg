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
        return @{666}
    }
}

//#[Swag.printbc]//, Swag.optim("", false)]
func toto()->[2] AA
{
    return @[AA.from(), AA.from()]
}

//#[Swag.printbc]
#test
{
    x := toto()
    @assert(x[0].x == 666)
    @assert(x[1].x == 666)
}