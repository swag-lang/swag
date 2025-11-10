func toto() {}

//#[Swag.PrintBc]
#test
{
    {
        let x = 666.1'f64
        let y = cast(s64) x
        @assert(y == 666)
    }
    {
        let x = 666'u64
        let y = cast(f64) x
        @assert(y == 666)
    }
}
