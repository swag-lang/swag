#global public
#global namespace Debug

func breakPanic(message: string, loc = #callerlocation)
{
    if Debugger.isAttached() Debugger.debugBreak()
    @panic(message, loc)
}

#[Swag.Inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr
            breakPanic(message, loc)
    }
}
