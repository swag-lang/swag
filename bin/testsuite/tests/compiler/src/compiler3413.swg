//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a, b := 666
    var v: closure(s32) = @|a, &b|(x: s32) { }
    //v(5)
}