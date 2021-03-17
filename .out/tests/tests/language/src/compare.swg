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
}