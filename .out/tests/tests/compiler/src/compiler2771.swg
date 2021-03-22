//#[swag.printbc, swag.optim("", false)]
#test
{
    a := 0xFFFFFFFF_FFFFFFFF'u64
    b := ~a
    @assert(b == 0)
}