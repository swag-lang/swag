using Core

#test
{
    var a: StaticArray'(s32, 50)
    for 5's32:
        a.add(#index)
    @assert(a.count == 5)
    @assert(a[0] == 0)
    @assert(a[4] == 4)

    a.sortReverse()
    @assert(a[0] == 4)
    @assert(a[4] == 0)

    a.sort()
    @assert(a[0] == 0)
    @assert(a[4] == 4)

    @assert(a.front() == 0)
    @assert(a.back() == 4)
}
