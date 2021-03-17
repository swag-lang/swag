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

//#[swag.printbc]
#test
{
    var x: [2] AA
    y := x
    y = x
}