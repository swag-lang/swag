//#global testerror "Err0194"

func(T) toto(x: s32, y: T) {}

//#[Swag.PrintBc]
#test
{
    toto(0's32, 1)
}
