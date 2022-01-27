#global testerror

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a, b := 666
    var v: closure(s32) = @|a,|(x: s32) { }
}