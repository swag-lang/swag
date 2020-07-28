func toto(x: s64, y: string)
{
    loop 2
    {
        @print(x)
        @print(y)
    }
}

//#[swag.printbc]
#test
{
    toto(10, "myString")
}