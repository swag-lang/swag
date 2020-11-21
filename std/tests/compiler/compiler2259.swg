//#[swag.printbc]
#test
{
    {
        x := -1's8
        @assert(@atomand(&x, 3) == -1)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        x := -1's16
        @assert(@atomand(&x, 3) == -1)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        x := -1's32
        @assert(@atomand(&x, 3) == -1)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }

    {
        x := -1's64
        @assert(@atomand(&x, 3) == -1)
        @assert(@atomand(&x, 2) == 3)
        @assert(x == 2)
    }
}