#import "std"
using std

#test
{
    var rand: random.Random
    var arr: Array's32

    rand.seed(0)
    loop 10
        arr.add(rand.nextS32(0, 100))

    arr.sort(@(x, y) => y <=> x)
    @assert(!sort.isSorted(@mkslice(arr.buffer, arr.count)))
    arr.sort()
    @assert(sort.isSorted(@mkslice(arr.buffer, arr.count)))
}