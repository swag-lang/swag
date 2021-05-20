
using Core.Console

//#[Swag.printbc]
#test
{
    silent(true)
    ptr := &print
    ptr("toto")
    silent(false)
}