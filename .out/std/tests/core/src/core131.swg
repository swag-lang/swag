
using core

#test
{
    var rand: Random.Rng
    var arr: Array's32

    rand.seedU32(0)
    loop 10
        arr.add(rand.nextS32(0, 100))

    arr.sort(@(x, y) => y <=> x)
    @assert(!Slice.isSorted(@mkslice(arr.buffer, cast(uint) arr.count)))
    arr.sort()
    @assert(Slice.isSorted(@mkslice(arr.buffer, cast(uint) arr.count)))
}