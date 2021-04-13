func print(params: ...)
{
}

//#[swag.printbc]
#test
{
    ptr := &print
    ptr("toto")
}