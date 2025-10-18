#global testerror "Err0579"

enum OT
{
    A = 1
    B
}

//#[Swag.PrintBc]
#test
{
    var pt = #type { x: OT }
}
