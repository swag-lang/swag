func toto(p: s64)
{
    @print(p)
}

#[swag.printbc]
#test
{
    ptr := &toto
    ptr(666)
}