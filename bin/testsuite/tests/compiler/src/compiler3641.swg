//#[Swag.PrintBc]
#test
{
    const t = [10, 10, 10]

    var x = 0'f32
    loop i in 3
    {
        defer x += t[i]

        if i < 3 and t[i] == 9:
            continue

        @assert(i != 0 or x == 0)
        @assert(i != 1 or x == 10)
        @assert(i != 2 or x == 20)
    }
}