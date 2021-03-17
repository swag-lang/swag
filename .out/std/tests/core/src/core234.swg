
using core

//#[swag.printbc]
func toto() throw
{
    throw format.toString("toto%", 10)
}

#test
{
    catch toto()
    @assert(@err == "toto10")
}