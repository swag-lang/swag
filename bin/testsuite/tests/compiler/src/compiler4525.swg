struct A { r, g, b, d : s32; }

#[Swag.Strict]
typealias AL = A

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var a: AL = {1, 2, 3, 4}
}