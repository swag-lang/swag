#test
{
    var array: [?] s32 = [1, 2, 3, 4]
    var result = 0

    foreach value in array where value & 1 == 0 do
        result += value

    @assert(result == 6)
}
