#[swag.mixin]
func mac()->s32
{
    return 1
}

#test
{
    a := mac()
    @assert(a == 1)
}