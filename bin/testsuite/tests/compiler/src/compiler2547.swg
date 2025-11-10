struct AA
{
    x:   *u8
}

impl AA
{
    func opDrop(me)
    {
        //@print("drop\n")
    }
}

//#[Swag.PrintBc]
func titi()->[1] AA
{
    var ptrptr: *u8
    return [{ptrptr}]
}

//#[Swag.PrintBc]
#test
{
    var v = titi()
}
