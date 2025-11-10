//#[Swag.PrintBc]
#test
{
    var a = 0'u64
    foreach "ABC"
    {
        a += @index
    }

    @assert(a == 3)
}
