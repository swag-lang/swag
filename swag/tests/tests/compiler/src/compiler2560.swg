struct AA
{
    x = 0
}

impl AA
{
    func opDrop(self)
    {
        @print("drop")
    }

    func opPostMove(self)
    {
        @print("move")
    }

    func from()->AA
    {
        return @{666}
    }
}

#[swag.printbc]
func toto()->[2] AA
{
    return @[AA.from(), AA.from()]
}

//#[swag.printbc]
#test
{
    x := toto()
}