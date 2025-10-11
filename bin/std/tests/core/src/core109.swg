using Core

#test
{
    var x: Array's32
    for 5 do
        x.add(cast(s32) @index)

    var total = 0
    foreach v in x do
        total += v
    @assert(total == 10)

    foreach &v in x do
        dref v = 5

    total = 0
    foreach v in x do
        total += v
    @assert(total == 25)
}
