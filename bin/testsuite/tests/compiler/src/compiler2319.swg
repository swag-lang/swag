#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: code)
{
    var prev = @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    @getcontext().hasError = 666

    #macro
    {
        #mixin #up stmt
    }
}

#test
{
    var cxt = dref @getcontext()
    pushContext(cxt)
    {
        @assert(@getcontext().hasError == 666)
    }

    @assert(@getcontext().hasError == 0)
}