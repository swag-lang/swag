//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let a = 666
    var b = 667
    let v: closure(s32) = closure|a, &b|(x: s32)
    {
    }
    v(5)
}