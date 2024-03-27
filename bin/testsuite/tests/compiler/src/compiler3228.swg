#test
{
    loop i: 4
    {
        switch i
        {
        case 0, 3:
            continue
        }

        @assert(i == 1 or i == 2)
    }
}
