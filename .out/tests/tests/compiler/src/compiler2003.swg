#test
{
    a := @{1, 2}
    b := @{1, 2}
    a.item0 += 10
    @assert(a.item0 == 11)
}
