#[Swag.Optim("", false)]
#test
{
    {
        var a = 1'u8
        var b = 3'u8
        var c = a ^ b
        @assert(c == 2)
    }

    {
        var a = 1'u16
        var b = 3'u16
        var c = a ^ b
        @assert(c == 2)
    }
}