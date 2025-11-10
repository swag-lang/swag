#[Swag.ConstExpr]
func getType(x: s32)->typeinfo
{
    if x == 0 do
        return s32
    else do
        return bool
}

#test
{
    var x: #decltype(getType(0))
    @assert(#typeof(x) == s32)
    var x1: #decltype(getType(1))
    @assert(#typeof(x1) == bool)
}
