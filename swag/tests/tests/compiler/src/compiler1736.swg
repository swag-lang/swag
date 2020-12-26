//#[swag.printbc]
#test
{
    a := 0'uint
    visit "ABC"
    {
        a += @index
    }

    @assert(a == 3)
}