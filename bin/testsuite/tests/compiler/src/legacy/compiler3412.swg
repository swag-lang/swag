#global testerror "Err0400"

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var a, b = 666
    var v: func||(s32) = func|a,|(x: s32) { }
}