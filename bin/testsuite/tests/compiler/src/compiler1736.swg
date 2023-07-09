//#[Swag.PrintBc]
#test
{
    var a = 0'u64
    visit "ABC"
    {
        a += @index
    }

    @assert(a == 3)
}