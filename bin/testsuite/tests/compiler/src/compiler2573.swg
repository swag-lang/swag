//#global testerror
struct ErrTiti{}

func toto() throw
{
    throw ErrTiti{}
}

func titi() throw
{
    catch toto()
    if @err() != null: throw ErrTiti{}
}

#test
{
    catch titi()
    @assert(@err() != null)
}