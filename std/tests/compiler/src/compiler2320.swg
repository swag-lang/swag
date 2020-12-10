#[swag.macro]
func pushContext(cxt: swag.Context, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    @getcontext().errorMsgLen = 666

    #macro
    {
        #mixin `stmt
    }
}

#test
{
    x := @[0, 0]
    y := @[1, 2]
    @memmove(&x, &y, cast(uint) @sizeof(x))
    @assert(x[0] == 1)
    @assert(x[1] == 2)
}