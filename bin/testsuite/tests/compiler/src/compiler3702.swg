#[Swag.Optim("", false)]
#test
{
    {
        a := 1'u8
        b := 3'u8
        c := a ^ b
        @assert(c == 2)
    }

    {
        a := 1'u16
        b := 3'u16
        c := a ^ b
        @assert(c == 2)
    }
}