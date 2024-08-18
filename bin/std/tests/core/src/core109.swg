using Core

#test
{
    var x: Array's32
    loop 5:
        x.add(cast(s32) #index)

    var total = 0
    visit v in x:
        total += v
    @assert(total == 10)

    visit &v in x:
        dref v = 5

    total = 0
    visit v in x:
        total += v
    @assert(total == 25)
}
