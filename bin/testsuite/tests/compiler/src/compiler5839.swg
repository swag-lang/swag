//#global skip
#[Swag.Mixin]
func safety(expr: #code bool, message: string = null, loc = #callerlocation)
{
    let b = #inject(expr)
    if !b:
        @panic(message, loc)
    @assert(b)
}

#[Swag.Inline]
func toto(val5839: s32)
{
    safety(val5839 == 55)
}

#[Swag.Optimize("", false)]
#test
{
    toto(55)
}