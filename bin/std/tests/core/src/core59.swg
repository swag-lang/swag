using Core

//#[Swag.PrintBc]
func fromCapa(capa: u64)->BitArray
{
    var x: BitArray
    x.reserve(capa, false)
    return x
}

#test
{
    var x: BitArray = [true, false, true, false]
    var y: BitArray = fromCapa(4)

    visit val, idx in x
    {
        y[idx] = val
    }

    @assert(x == y)
}
