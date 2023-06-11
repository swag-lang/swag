
using Core

//#[Swag.PrintBc]
func toto() throw
{
    throw Format.toString("toto%", 10)
}

#test
{
    catch toto()
    @assert(@errmsg == "toto10")
}