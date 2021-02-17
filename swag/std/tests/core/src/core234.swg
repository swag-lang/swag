#import "core"
using core

//#[swag.printbc]
func toto()
{
    throw format.toString("toto%", 10)
}

#test
{
    catch toto()
    @assert(@err == "toto10")
}