#[Swag.Mixin]
func mac()->s32
{
    return 1
}

#test
{
    let a = mac()
    @assert(a == 1)
}
