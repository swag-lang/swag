#global public
#global namespace Debug

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !#inject(expr) do
            @panic(message, loc)
    }
}

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: Swag.SafetyWhat) safety(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(what)
    {
        if !#inject(expr) do
            @panic(message, loc)
    }
}

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func safetyBoundCheck(expr50: #code bool, message: string = null, loc = #callerlocation)
{
    safety'(Swag.SafetyWhat.BoundCheck)(expr50, message, loc)
}
