#test
{
    var x = 0
    #scope A
    {
        loop 10
        {
            #scope A
            {
                loop 10
                {
                    x += 1
                    break A
                }
            }

            x += 1
            break A
        }
    }

    @assert(x == 2)
}