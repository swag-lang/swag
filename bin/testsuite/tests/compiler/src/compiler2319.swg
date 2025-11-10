#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: #code void)
{
    let prev = @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    @getcontext().hasError = 666

    #macro
    {
        #inject(#up stmt)
    }
}

#test
{
    let cxt = dref @getcontext()
    pushContext(cxt)
    {
        @assert(@getcontext().hasError == 666)
    }

    @assert(@getcontext().hasError == 0)
}
