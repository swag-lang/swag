#global testerror
func toto(v: ^s32)
{
}

//#[Swag.PrintBc]
#test
{
    x := 0
    ptr := &x
    toto(ptr)
}
