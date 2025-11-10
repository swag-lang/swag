//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let a  = 666
    var b  = 667
    let v: func||(s32) = func|a, &b|(x: s32) {}
    v(5)
}
