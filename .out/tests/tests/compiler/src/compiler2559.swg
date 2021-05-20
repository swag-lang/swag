struct AA
{
    x = 0
}

impl AA
{
    /*func opPostCopy(self)
    {
@print("copy")
    }*/
}

//#[Swag.printbc]
func toto()->[2] AA
{
    return @[{1},{1}]
}

//#[Swag.printbc]
#test
{
    x := toto()
}