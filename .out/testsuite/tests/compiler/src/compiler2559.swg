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

//#[Swag.PrintBc]
func toto()->[2] AA
{
    return @[{1},{1}]
}

//#[Swag.PrintBc]
#test
{
    x := toto()
}