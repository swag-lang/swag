//#[Swag.printbc]
#test
{
    a := 0'u32
    visit "ABC"
    {
        switch @index
        {
            case 0: break
            case 1: break
        }

        a += 1
    }

    @assert(a == 3)
}