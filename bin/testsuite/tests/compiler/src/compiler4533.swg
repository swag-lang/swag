//#global testerror

#[Swag.Strict]
typealias B = func(s32)

var G = 0

#test
{
    let x: B = func(a) { G += a; }
    x(1)
    @assert(G == 1)
}
