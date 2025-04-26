#global public
#global namespace Debug

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr:
            @panic(#self.message, #self.loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: string) safety(expr: bool, message: string = null, loc = #callerlocation)
{
    #if #safety(what)
    {
        if !expr:
            @panic(message, loc)
    }
}

#[Swag.Inline, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func safetyBoundCheck(expr: bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}
