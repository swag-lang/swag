func toto(p: ...)
{
    @print(cast(string) p[0])
}

#[swag.printbc]
#test
{
    a := "toto"
    b := a[0]
    toto(a, b)
}