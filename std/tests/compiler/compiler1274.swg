macro toto()
{
    //a += 2
}

//#[swag.printbc]
#test
{
    a := 0
    toto()
    @print(cast(s64) a)
}