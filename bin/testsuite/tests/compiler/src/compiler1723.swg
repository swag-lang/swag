#test
{
    #[Swag.EnumFlags]
    enum MyFlags: u32
    {
        A
        B
        C
        D
    }

    @assert(MyFlags.A == 0b00000001)
    @assert(MyFlags.B == 0b00000010)
    @assert(MyFlags.C == 0b00000100)
    @assert(MyFlags.D == 0b00001000)

    let value = MyFlags.B | MyFlags.C
    @assert(value == 0b00000110)
    @assert(value & MyFlags.B == MyFlags.B)
    @assert(value & MyFlags.C == MyFlags.C)
    @assert(cast()(value & MyFlags.B))
}