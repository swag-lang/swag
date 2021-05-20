func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[Swag.printbc]
#test
{
    x := catch toto()
}