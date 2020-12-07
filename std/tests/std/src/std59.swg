#import "std"
using std

//#[swag.printbc]
func fromCapa(capa: u32)->BitArray
{
    var x: BitArray
    x.reserve(capa, false)
    return x
}

//#[swag.printbc]
#test
{
    var x: BitArray = @[true, false, true, false]
    var y: BitArray = fromCapa(4)

    visit val, idx: x
    {
        y[idx] = val
    }

    @assert(x == y)
}