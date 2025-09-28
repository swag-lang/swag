#global testerror "Err0315"

struct ErrTiti{}

#[Swag.Macro]
func toto(stmt: #code void)
{
    #macro
    {
        #inject(#up stmt)
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
    @assert(@err != null)
}