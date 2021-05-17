
using core

#test
{
    var rand: random.Rng
    var arr: Array's32

    rand.seedU32(0)
    loop 10
        arr.add(rand.nextS32(0, 100))

    arr.sort(@(x, y) => y <=> x)
    @assert(!slice.isSorted(@mkslice(arr.buffer, cast(uint) arr.count)))
    arr.sort()
    @assert(slice.isSorted(@mkslice(arr.buffer, cast(uint) arr.count)))
}