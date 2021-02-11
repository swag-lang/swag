struct AA
{
    x: *u8
}

impl AA
{
    func opDrop(using self)
    {
        //@print("drop\n")
    }
}

//#[swag.printbc]
func titi()->[1] AA
{
    var ptrptr: *u8
    return @[{ptrptr}]
}

//#[swag.printbc]
#test
{
    v := titi()
}
