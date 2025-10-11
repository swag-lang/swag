#test
{
    const O = [{dl: 1, dx: 2, dy: 3}, {1, 2, 3}]

    @assert(O[0].item0 == 1)
    @assert(O[0].item1 == 2)
    @assert(O[0].item2 == 3)

    @assert(O[1].item0 == 1)
    @assert(O[1].item1 == 2)
    @assert(O[1].item2 == 3)
}
