#global public
#global namespace Debug

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func assert(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !#inject expr:
            @panic(#self.message, #self.loc)
    }
}

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: string) safety(expr51: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(what)
    {
        if !#inject expr51:
            @panic(message, loc)
    }
}

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func safetyBoundCheck(expr50: #code bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr50, message, loc)
}
