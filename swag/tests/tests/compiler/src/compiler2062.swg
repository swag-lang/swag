#test
{
    x := @typeof(s32)
    y := @typeof(s32)
    v := @typecmp(x,y, swag.TypeCmpFlags.Strict)
    @assert(v)
}