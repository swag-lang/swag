using Core.Console

//#[Swag.PrintBc]
#test
{
    silent(true)
    let ptr = &print
    ptr("toto")
    silent(false)
}
