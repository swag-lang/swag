#global testerror "Err0457"

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var a, b = 666
    var v: closure(s32) = closure|a,|(x: s32) { }
}