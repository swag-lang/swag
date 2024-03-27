using Swag

//#[PrintBc]
func test2()->string
{
    return "toto1"
}

//#[PrintBc]
#test
{
    var res = test2()
}
