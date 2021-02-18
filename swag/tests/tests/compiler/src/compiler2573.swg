func toto()
{
    throw "titi"
}

func titi()
{
    catch toto()
    if @err throw "super titi"
}

#test
{
    assume titi()
}