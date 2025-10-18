#global testerror "Err0044"
var G = 0

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var v: func(s32) = func||(x: s32) { G = x; }
}
