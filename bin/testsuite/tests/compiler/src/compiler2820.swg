alias X = [2] u32
func toto(x: X)
{
    x[0] = 1
    ptr := &x[0]
    @assert(@stringof(ptr) == "*u32")
}

#test
{
    var x: [2] u32
    toto(x)
}