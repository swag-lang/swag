func toto()
{

}

//#[Swag.printbc]
#test
{
    {
        x := 666.1'f64
        y := cast(s64) x
        @assert(y == 666)
    }
    {
        x := 666'u64
        y := cast(f64) x
        @assert(y == 666)
    }
}