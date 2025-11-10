struct AA {}

impl AA
{
    func opDrop(me)
    {
        //@print("drop")
    }
}

//#[Swag.PrintBc]
#test
{
    var x: [2] AA
    var y  = x
    y = x
}
