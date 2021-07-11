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

// Push a new allocator for the given block of code
#[Swag.macro]
func pushAllocator(allocator: Swag.IAllocator, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    cxt := dref prev
    cxt.allocator = allocator
    @setcontext(cxt)

    #macro
    {
        #mixin `stmt
    }
}