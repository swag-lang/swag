#global testerror "Err0882"
//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a: [100] s32
    var v: closure(s32) = closure|a|(x: s32) { }
    //v(5)
}