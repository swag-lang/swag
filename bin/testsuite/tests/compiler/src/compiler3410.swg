#global testerror "Err0647"
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: func(s32) = closure||(x: s32) { G = x; }
}