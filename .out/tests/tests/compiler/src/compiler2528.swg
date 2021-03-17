func toto()->s64 throw
{
    throw "toto"
    return 555
}

//#[swag.printbc]
#test
{
    x := catch toto()
}