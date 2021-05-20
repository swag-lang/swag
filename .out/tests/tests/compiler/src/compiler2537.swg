struct AA
{
    x = 0
}

//#[Swag.printbc]
func toto()->AA throw
{
    throw "toto"
    return @{1}
}

//#[Swag.printbc]
func titi()->AA throw
{
    return try toto()
}

#test
{
    x := catch titi()
    @assert(@err == "toto")
}