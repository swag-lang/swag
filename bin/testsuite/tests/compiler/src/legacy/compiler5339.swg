#[Swag.Optimize(.All, false)]
#test
{
    {
        var a = 1's8
        a -= 2
        let b = @abs(a)
        @assert(b == 1)
    }

    {
        var a = 1's16
        a -= 2
        let b = @abs(a)
        @assert(b == 1)
    }

    {
        var a = 1's32
        a -= 2
        let b = @abs(a)
        @assert(b == 1)
    }

    {
        var a = 1's64
        a -= 2
        let b = @abs(a)
        @assert(b == 1)
    }
}
