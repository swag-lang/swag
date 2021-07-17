func toto(x: string~64)
{
    @print("x")
}

#[Swag.PrintBc]
#test
{
    toto("titi")
}