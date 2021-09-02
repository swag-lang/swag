struct AA
{
    x = 0
}

//#[Swag.PrintBc]
func toto()->AA throw
{
    throw "toto"
    return @{1}
}

//#[Swag.PrintBc]
func titi()->AA throw
{
    return try toto()
}

#test
{
    x := catch titi()
    @assert(@err == "toto")
}