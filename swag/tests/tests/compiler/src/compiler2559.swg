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

//#[swag.printbc]
func toto()->[2] AA
{
    return @[{1},{1}]
}

//#[swag.printbc]
#test
{
    x := toto()
}