
using core

//#[Swag.printbc]
func toto() throw
{
    throw Format.toString("toto%", 10)
}

#test
{
    catch toto()
    @assert(@err == "toto10")
}