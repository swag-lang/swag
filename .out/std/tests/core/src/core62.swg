
using core.console

//#[Swag.printbc]
#test
{
    silent(true)
    ptr := &print
    ptr("toto")
    silent(false)
}