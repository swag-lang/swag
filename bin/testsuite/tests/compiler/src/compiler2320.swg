#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: #code void)
{
    var prev = @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    @getcontext().errorMsgLen = 666

    #macro
    {
        #inject(#up stmt)
    }
}

#test
{
    var x = [0, 0]
    var y = [1, 2]
    @memmove(&x, &y, cast(u64) #sizeof(x))
    @assert(x[0] == 1)
    @assert(x[1] == 2)
}
