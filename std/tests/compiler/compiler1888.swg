//#[swag.printbc]
func toto(x: s64)
{
    @print(x)
}

#[swag.printbc]
func titi()
{
    x := 666's64
    #run toto(x)
}
