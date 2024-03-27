struct AA
{
    x = 0
}

impl AA
{
}

//#[Swag.PrintBc]
func toto()->[2] AA
{
    return [{1}, {1}]
}

//#[Swag.PrintBc]
#test
{
    var x = toto()
}
