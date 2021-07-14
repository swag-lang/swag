struct AA
{

}

impl AA
{
    func opDrop(self)
    {
        //@print("drop")
    }
}

//#[Swag.PrintBc]
#test
{
    var x: [2] AA
    y := x
    y = x
}