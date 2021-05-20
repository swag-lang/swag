#[Swag.macro]
func pushContext(cxt: Swag.Context, stmt: code)
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
    cxt := dref @getcontext()
    pushContext(cxt)
    {
        @assert(@getcontext().errorMsgLen == 666)
    }

    @assert(@getcontext().errorMsgLen == 0)
}