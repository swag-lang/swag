struct A { r, g, b, d: s32 }

#[Swag.Strict]
alias AL = A

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    var a: AL = {1, 2, 3, 4}
}
