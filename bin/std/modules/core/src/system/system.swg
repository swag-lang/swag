#global public
#global namespace System

// Push a new execution context for the given block of code
#[Swag.Macro]
func pushContext(cxt: Swag.Context, stmt: code)
{
    var prev = @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    #macro
    {
        #mixin #up stmt
    }
}