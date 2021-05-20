
using core

//#[Swag.printbc]
#test
{
    x := ".5"
    y := try Parse.getF64(x)
    @assert(y.value == 0.5)
}