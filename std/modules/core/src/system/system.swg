public namespace system;

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

// Convert a lambda pointer to an external callback pointer
// that can be called from a foreign library
func(T) lambdaToCallback(lambda: func(T))->const *void
{
    var l64 = cast(uint) lambda
    if l64 & swag.LambdaBcMarker
        l64 = autocast @thrunptr()
    else
        l64 &= ~swag.LambdaForeignMarker
    return autocast l64
}