//#[swag.printbc]
#test
{
    a := "toto"

    start := 1'u32
    end := 2'u32
    c := a[start..end]
    @assert(c == "ot")
}