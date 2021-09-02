
using Core

#test
{
    var x: Array's32
    loop 5
        x.add(cast(s32) @index)

    total := 0
    visit v: x
        total += v
    @assert(total == 10)

    visit *v: x
        v[0] = 5

    total = 0
    visit v: x
        total += v
    @assert(total == 25)
}