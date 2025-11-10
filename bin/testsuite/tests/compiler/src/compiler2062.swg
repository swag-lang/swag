#test
{
    let x = #typeof(s32)
    let y = #typeof(s32)
    let v = @typecmp(x, y, Swag.TypeCmpFlags.Strict)
    @assert(v)
}
