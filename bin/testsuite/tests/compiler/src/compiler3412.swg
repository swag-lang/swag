#global testerror "Err0436"

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var a, b = 666
    var v: closure(s32) = closure|a,|(x: s32) { }
}