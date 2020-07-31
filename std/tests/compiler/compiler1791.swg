#test
{
    @assert(toto() == 556)
    @assert(toto() == 557)
}

#[swag.printbc]
func toto()->s32
{
    #[swag.global]
    var x = 555
    x += 1
    return x
}