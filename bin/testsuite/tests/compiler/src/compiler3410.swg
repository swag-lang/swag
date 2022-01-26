#global testerror
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: func(s32) = @||(x: s32) { G = x; }
}