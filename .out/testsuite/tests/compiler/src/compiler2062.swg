#test
{
    x := @typeof(s32)
    y := @typeof(s32)
    v := @typecmp(x,y, Swag.TypeCmpFlags.Strict)
    @assert(v)
}