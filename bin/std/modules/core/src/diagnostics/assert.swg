#global public
#global namespace Debug

func breakPanic(message: string, loc = #callerlocation)
{
    if Debugger.isAttached()
        Debugger.debugBreak()
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

#[Swag.Inline]
func(what: string) safety(expr: bool, message: string = null, loc = #callerlocation)
{
    #if @safety(what)
    {
        if !expr
            breakPanic(message, loc)
    }
}

#[Swag.Inline]
func safetyBoundCheck(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}

#[Swag.Inline]
func safetyOverflow(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("overflow")(expr, message, loc)
}

#[Swag.Inline]
func safetyMath(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("math")(expr, message, loc)
}

#[Swag.Inline]
func safetyCast(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("cast")(expr, message, loc)
}
