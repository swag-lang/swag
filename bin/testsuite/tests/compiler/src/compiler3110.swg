#global testerror "Err0138"

#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin #up stmt
    }
}

func titi() throw
{
    throw "titi"
}

//#[Swag.PrintBc, Swag.Optim("", false)]
func tata()
{
    toto()
    {
        try titi()
    }
}

#test
{
    catch tata()
    @assert(@err() != null)
}