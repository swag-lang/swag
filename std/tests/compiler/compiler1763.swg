func a(x: ...)
{
    @print(cast(s64) x[0])
    @print(@kindof(x[1]).name)
}

#[swag.printbc]
#test
{
    a(666's64, "toto")
}