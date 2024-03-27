//#global testerror

#[Swag.Strict]
typealias B = closure(s32)

var G = 0

#test
{
    let x: B = closure||(a)
    {
        G += a
    }
    x(1)
    @assert(G == 1)
}
