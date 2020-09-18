#import "std"
using std

#test
{
    let rand: random.Random
    let arr: Array's32

    rand.seed(0)
    loop 100
        arr.add(rand.nextS32(0, 100))

    arr.sort(@(x, y) => y <=> x)
    @assert(sort.isSorted(@mkslice(arr.buffer, arr.count), false))
}