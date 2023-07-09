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
    var y = x
    y = x
}