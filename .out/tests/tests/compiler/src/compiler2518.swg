var t: const [..] u16 = @[1,2,3,4]

#test
{
    @assert(@countof(t) == 4)
    @assert(t[0] == 1)
    @assert(t[1] == 2)
    @assert(t[2] == 3)
    @assert(t[3] == 4)
}