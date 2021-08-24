#global testerror

#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin `stmt
    }
}

func titi() throw
{
    throw "titi"
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    toto()
    {
        try titi()
    }
}