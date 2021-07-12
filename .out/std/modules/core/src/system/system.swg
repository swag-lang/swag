#global public
#global namespace System

// Push a new execution context for the given block of code
#[Swag.macro]
func pushContext(cxt: Swag.Context, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    @setcontext(cxt)
    #macro
    {
        #mixin `stmt
    }
}