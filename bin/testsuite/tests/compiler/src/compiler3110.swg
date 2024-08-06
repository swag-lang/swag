#global testerror "Err0391"

struct ErrTiti{}

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
    throw ErrTiti{}
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