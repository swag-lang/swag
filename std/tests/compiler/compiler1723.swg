#test
{
    #[swag.enumflags]
    enum MyFlags: u32
    {
        A
        B
        C
        D
    }

    @assert(MyFlags.A == 0b00000000)
    @assert(MyFlags.B == 0b00000001)
    @assert(MyFlags.C == 0b00000010)
    @assert(MyFlags.D == 0b00000100)

    value := MyFlags.B | MyFlags.C
    @assert(value == 0b00000011)
    @assert(value & MyFlags.B == MyFlags.B)
    @assert(value & MyFlags.C == MyFlags.C)
    @assert(acast(value & MyFlags.B))
}