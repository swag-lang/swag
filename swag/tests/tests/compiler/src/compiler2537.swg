struct AA
{
    x = 0
}

//#[swag.printbc]
func toto()->AA throw
{
    throw "toto"
    return @{1}
}

//#[swag.printbc]
func titi()->AA throw
{
    return try toto()
}

#test
{
    x := catch titi()
    @assert(@err == "toto")
}