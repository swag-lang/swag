//#[swag.printbc]
#test
{
    a := 0'u32
    visit "ABC"
    {
        a += @index
    }

    @assert(a == 3)
}