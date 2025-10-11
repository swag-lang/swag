var G = 0

#[Swag.Macro]
func assert(expr: #code s32)
{
    //@print("before")
    let b = #inject(expr)
    @assert(b == G)
    //@print("after")
}

#[Swag.NoInline]
func xx(val: s32)->s32
{
    return val
}

//#[Swag.PrintBc]
#test
{
    G = 55
    assert(xx(55))
    G = 44
    assert(xx(44))
}
