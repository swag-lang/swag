#global public
#global namespace System

// Push a new execution context for the given block of code
#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: #code void)
{
    var prev = @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    #macro
    {
        #inject(#up stmt)
    }
}
