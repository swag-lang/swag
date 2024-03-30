using Core

#test
{
    var x = 0b00000000_00110101'u16
    x = Math.reverse(x)
    @assert(x == 0b10101100_00000000)
}

#test
{
    {
        var x = 0b00000001'u8
        x = Math.reverse(x)
        @assert(x == 0b10000000)
    }

    {
        var x = 0b00110101'u8
        x = Math.reverse(x)
        @assert(x == 0b10101100)
    }

    {
        var x = 0b00110101_00000000'u16
        x = Math.reverse(x)
        @assert(x == 0b00000000_10101100)
    }

    {
        var x = 0b1'u32
        x = Math.reverse(x)
        @assert(x == 0b10000000_00000000_00000000_00000000)
    }

    {
        var x = 0b10000000_10000000_00000000_00000000'u32
        x = Math.reverse(x)
        @assert(x == 0b00000000_00000000_00000001_00000001)
    }

    {
        var x = 0b11000000_10000000_00000000_00000000_00000000_00000000_00000000_00000000'u64
        x = Math.reverse(x)
        @assert(x == 0b00000000_00000000_00000000_00000000_00000000_00000000_00000001_00000011)
    }
}
