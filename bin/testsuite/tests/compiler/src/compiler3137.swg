#test
{
    var x = 0
    #scope A
    {
        for 10
        {
            #scope A
            {
                for 10
                {
                    x += 1
                    break in A
                }
            }

            x += 1
            break in A
        }
    }

    @assert(x == 2)
}