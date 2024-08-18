#global public
#global namespace Debug

func breakPanic(message: string, loc = #callerlocation)
{
    if Debugger.isAttached():
        Debugger.debugBreak()
    @panic(message, loc)
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr:
            breakPanic(message, loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: string) safety(expr: bool, message: string = null, loc = #callerlocation)
{
    #if @safety(what)
    {
        if !expr:
            breakPanic(message, loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func safetyBoundCheck(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}
