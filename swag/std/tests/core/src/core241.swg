#import "core"
using core

//#[swag.printbc]
#test
{
	var x: Array'u32
    x.resize(100)

    jobsystem.parallelFor(x.toSlice(), 1, null)
    {
        buffer[0] = 666
    }

    loop x @assert(x[@index] == 666)
}