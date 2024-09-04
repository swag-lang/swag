#global testerror "Err0515"
var G = 0

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var v: func(s32) = func||(x: s32) { G = x; }
}