//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 666
    b := 667
    var v: closure(s32) = closure|a, &b|(x: s32)
    {
    }
    v(5)
}