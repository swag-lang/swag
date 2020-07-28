#[swag.printbc]
func toto(x: s64, y: string)
{
    z := "this is a string"
    @print(x)
    @print(y)
}

#test
{
    toto(10, "myString")
}