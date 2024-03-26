#global public
#global namespace Debug

func breakPanic(message: string, loc = #callerlocation)
{
    if Debugger.isAttached() do
        Debugger.debugBreak()
    @panic(message, loc)
}

#[Swag.Inline, Swag.Warn("Wrn0006", Swag.WarnLevel.Disable)]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr do
            breakPanic(message, loc)
    }
}

#[Swag.Inline, Swag.Warn("Wrn0006", Swag.WarnLevel.Disable)]
func(what: string) safety(expr: bool, message: string = null, loc = #callerlocation)
{
    #if @safety(what)
    {
        if !expr do
            breakPanic(message, loc)
    }
}

#[Swag.Inline, Swag.Warn("Wrn0006", Swag.WarnLevel.Disable)]
func safetyBoundCheck(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}
