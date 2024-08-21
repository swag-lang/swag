#test
{
    var array: [] s32 = [1, 2, 3, 4]
    var result = 0

    visit value in array where value & 1 == 0:
        result += value

    @assert(result == 6)
}
