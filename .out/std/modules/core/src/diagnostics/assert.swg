#global public
#global namespace Debug

#[Swag.inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr
        {
            if Debugger.isAttached() Debugger.debugBreak()
            @panic(message, loc)
        }
    }
}
