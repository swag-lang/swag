#test
{
    var t = 0
    visit i: [4, 5]
    {
        t += i
    }
    @assert(t == 9)
}
