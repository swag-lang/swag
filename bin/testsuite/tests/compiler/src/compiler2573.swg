//#global testerror

func toto() throw
{
    throw "titi"
}

func titi() throw
{
    catch toto()
    if @errmsg throw "super titi"
}

#test
{
    catch titi()
    @assert(@errmsg != null)
}