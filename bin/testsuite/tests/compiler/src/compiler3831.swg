#[Swag.Optimize(.All, true)]
#test
{
    {
        var x = 666
        @init(&x, 1)(555)
        @assert(x == 555)
    }

    {
        var x = [1, 2]
        @init(&x, 2)(555)
        @assert(x[0] == 555)
        @assert(x[1] == 555)
    }
}
