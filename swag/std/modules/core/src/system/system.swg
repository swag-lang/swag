#global public
#global namespace system

// Push a new execution context for the given block of code
#[swag.macro]
func pushContext(cxt: swag.Context, stmt: code)
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
#[swag.macro]
func pushAllocator(allocator: swag.IAllocator, stmt: code)
{
    prev := @getcontext()
    defer @setcontext(prev)

    cxt := :prev
    cxt.allocator = allocator
    @setcontext(cxt)

    #macro
    {
        #mixin `stmt
    }
}