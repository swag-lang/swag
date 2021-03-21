const AA: const [..] s32 = @[1, 2, 3, 4, 5, 6, 7, 8, 9]
#test
{
    x := AA
    @assert(@sizeof(x) == 2 * @sizeof(*void))
    @assert(@typeof(x) == const [..] s32)
    @assert(x[0] == 1)
    @assert(x[8] == 9)
}