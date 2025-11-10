#[Swag.Optimize(.All, false)]
func toto(v: s32)
{
    var x: [255] u8
    {
        let a = 5's8
        let b = a % v
        @assert(b == 1)
    }

    {
        let a = 5's16
        let b = a % v
        @assert(b == 1)
    }

    {
        let a = 5's32
        let b = a % v
        @assert(b == 1)
    }

    {
        let a = 5's64
        let b = a % v
        @assert(b == 1)
    }
}

#test
{
    toto(2)
}
