
using core

//#[swag.printbc]
#test
{
    x := ".5"
    y := try strconv.parseF64(x)
    @assert(y.value == 0.5)
}