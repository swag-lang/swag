#global testerror "Err0482"

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a, b = 666
    var v: closure(s32) = closure|a,|(x: s32) { }
}