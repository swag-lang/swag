
using Core.Console

//#[Swag.PrintBc]
#test
{
    silent(true)
    var ptr = &print
    ptr("toto")
    silent(false)
}