#test
{
    // Basic comparison operators
    {
        x := 5
        y := 10
        @assert(x == 5)
        @assert(x != 10)
        @assert(x <= 5)
        @assert(x < 10)
        @assert(x >= 5)
        @assert(x > 0)
    }

    // You can define operators multiple times
    {
        x := 5
        y := 10

        @assert(0 < x < 10)
        // equivalent to:
        @assert((0 < x) && (x < 10))

        @assert(5 == x != y)
        // equivalent to:
        @assert((5 == x) && (x != y))

        @assert(0 < x < 8 < y <= 10)
        // equivalent to:
        @assert((0 < x) && (x < 8) && (8 < y) && (y <= 10))
    }
}