#[Swag.Mixin]
func mac()->s32
{
    return 1
}

#test
{
    var a = mac()
    @assert(a == 1)
}