using Core

#test
{
    var x: Array's32
    for 5:
        x.add(cast(s32) #index)

    var total = 0
    foreach v in x:
        total += v
    @assert(total == 10)

    foreach &v in x:
        dref v = 5

    total = 0
    foreach v in x:
        total += v
    @assert(total == 25)
}
