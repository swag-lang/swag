#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    @getcontext().errorMsgLen = 666

    #macro
    {
        #mixin #up stmt
    }
}

#test
{
    x := [0, 0]
    y := [1, 2]
    @memmove(&x, &y, cast(u64) @sizeof(x))
    @assert(x[0] == 1)
    @assert(x[1] == 2)
}