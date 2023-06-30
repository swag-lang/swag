//#global testerror
#test
{
    struct X {}

    var x: closure(X, ...) = func(x: X, y: ...)
    {
        @assert(cast(s32) y[0] == 4)
    }

    var xx: X
    xx.x(4) // One day perhaps ?
}