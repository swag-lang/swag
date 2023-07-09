#test
{
    var x = @typeof(s32)
    var y = @typeof(s32)
    var v = @typecmp(x,y, Swag.TypeCmpFlags.Strict)
    @assert(v)
}