//#[Swag.PrintBc]
func test1()->string
{
    return "toto1"
}

//#[Swag.PrintBc]
#test
{
    let c = test1()
    @assert(c == "toto1")
}
