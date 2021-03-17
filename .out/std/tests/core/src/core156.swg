
using core

//#[swag.printbc]
#test
{
    x := ".5"
    y := strconv.parseF64(x)
    @assert(y.value == 0.5)
}