#global testerror "Err0570"

#[Swag.Strict]
alias B = func(s32)

var G = 0

#test
{
    var x: B = func||(a) { G += a; }
    x(1)
    @assert(G == 1)
}
