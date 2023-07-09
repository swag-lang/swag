//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a = 666
    var b = 667
    var v: closure(s32) = closure|a, &b|(x: s32)
    {
    }
    v(5)
}