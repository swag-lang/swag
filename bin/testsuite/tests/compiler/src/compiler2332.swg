#[Swag.Macro]
func toto()
{
    myVar := 0
    #macro(myVar)
    {
        alias myVar1 = myVar
        myVar1 += 1
    }

    @assert(myVar == 1)
}

#test
{
    toto()
}