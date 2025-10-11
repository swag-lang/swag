using Core

#test
{
    var rand: Random.Rng
    var arr:  Array's32

    rand.seedU32(0)
    for 10 do
        arr.add(rand.nextS32(0, 100))

    arr.sort(func(x, y) => y <=> x)
    @assert(!Slice.isSorted(@mkslice(arr.buffer, cast(u64) arr.count)))
    arr.sort()
    @assert(Slice.isSorted(@mkslice(arr.buffer, cast(u64) arr.count)))
}
