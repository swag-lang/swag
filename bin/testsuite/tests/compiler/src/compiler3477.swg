//#global testerror
//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    struct X {}

    let x: closure(X, ...) = func(x: X, y: ...)
    {
        @assert(cast(s32) y[0] == 4)
    }

    var xx: X
    xx.x(4) // One day perhaps ?
}