#global testerror "Err0595"

#[Swag.Strict]
typealias B = func(s32)

var G = 0

#test
{
    var x: B = closure||(a) { G += a; }
    x(1)
    @assert(G == 1)
}
