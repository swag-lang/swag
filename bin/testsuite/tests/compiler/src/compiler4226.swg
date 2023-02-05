//#global testerror

#[Swag.Macro]
func toto()
{
    #ast
    {
        return "#up a = 55"
    }
}

//#[Swag.PrintBc]
#test
{
    a := 0
    toto()
    @assert(a == 55)
}