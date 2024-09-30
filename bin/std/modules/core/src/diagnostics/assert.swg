#global public
#global namespace Debug

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: bool, message: nl string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr:
            @panic(message, loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: string) safety(expr: bool, message: nl string = null, loc = #callerlocation)
{
    #if #safety(what)
    {
        if !expr:
            @panic(message, loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func safetyBoundCheck(expr: bool, message: nl string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}
