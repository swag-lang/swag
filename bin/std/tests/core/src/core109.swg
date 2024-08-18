using Core

#test
{
    var x: Array's32
    loop 5 do
        x.add(cast(s32) #index)

    var total = 0
    visit v in x do
        total += v
    @assert(total == 10)

    visit &v in x do
        dref v = 5

    total = 0
    visit v in x do
        total += v
    @assert(total == 25)
}
