var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    let v: closure(s32) = func(x: s32)
    {
        G = x
    }
    v(5)
    @assert(G == 5)
}
