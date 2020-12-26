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

    // 3 ways comparison operator, returns a s32 of -1, 0 or 1
    {
        x1 := 10 <=> 20
        @assert(x1 == -1)
        x2 := 20 <=> 10
        @assert(x2 == 1)
        x3 := 20 <=> 20
        @assert(x3 == 0)
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