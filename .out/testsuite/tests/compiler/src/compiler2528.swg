func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[Swag.PrintBc]
#test
{
    x := catch toto()
}