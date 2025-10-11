using Core

//#[Swag.PrintBc]
#test
{
    var x: Array'u32
    x.resize(1)

    Jobs.parallelVisit(|buffer| x.toSlice())
    {
        if !buffer do
            return
        buffer[0] = 666
    }

    for x do
        @assert(x[@index] == 666)
}
