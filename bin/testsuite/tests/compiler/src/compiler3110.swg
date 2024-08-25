#global testerror "Err0324"

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

//#[Swag.PrintBc, Swag.Optimize("", false)]
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