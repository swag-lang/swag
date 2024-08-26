#global testerror "Err0564"
var G = 0

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var v: func(s32) = closure||(x: s32) { G = x; }
}