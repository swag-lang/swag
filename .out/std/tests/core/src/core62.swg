
using Core.Console

//#[Swag.PrintBc]
#test
{
    silent(true)
    ptr := &print
    ptr("toto")
    silent(false)
}