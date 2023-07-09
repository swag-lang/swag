func toto()
{

}

//#[Swag.PrintBc]
#test
{
    {
        var x = 666.1'f64
        var y = cast(s64) x
        @assert(y == 666)
    }
    {
        var x = 666'u64
        var y = cast(f64) x
        @assert(y == 666)
    }
}