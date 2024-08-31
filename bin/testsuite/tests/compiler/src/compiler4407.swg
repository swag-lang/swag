#global testerror "Err0605"
func toto(v: ^s32)
{
}

//#[Swag.PrintBc]
#test
{
    var x = 0
    var ptr = &x
    toto(ptr)
}
