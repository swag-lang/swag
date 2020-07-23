func toto(x: string, y: s64)
{
    @print(x)
    @print(y)
}

#[swag.printbc]
#test
{
    toto("toto", 10's64)
}