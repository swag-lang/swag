struct AA
{
    x = 0
}

func toto()->AA
{
    @seterr("t")
    return @{1}
}

//#[swag.printbc]
func titi()->AA
{
    return try toto()
}

#test
{
    x := catch titi()
}