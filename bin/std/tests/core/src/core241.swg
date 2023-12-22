
using Core

//#[Swag.PrintBc]
#test
{
	var x: Array'u32
    x.resize(1)

    Jobs.parallelVisit(|buffer| x.toSlice())
    {
        buffer[0] = 666
    }

    loop x do @assert(x[#index] == 666)
}