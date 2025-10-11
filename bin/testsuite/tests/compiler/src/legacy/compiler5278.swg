#test
{
    if const a = 1 do
        @assert(a == 1)
    if const a = 1
    {
        @assert(a == 1)
    }

    for var x = 0; x < 2; x += 1 do
        x = x
}
