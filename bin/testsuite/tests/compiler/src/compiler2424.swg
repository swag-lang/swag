func print(f: f32)
    verify
    {
        @assert(#isconstexpr(f))
        return true
    }
{
}

#[Swag.ConstExpr]
func tt() => 3.14

#test
{
    var x = 3.14
    print(tt())
}
