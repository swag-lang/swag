#global public
#global namespace Debug

#[Swag.Mixin]
func assert(expr: code, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        let #mix0 = #inject #self.expr
        if !#mix0:
            @panic(#self.message, #self.loc)
    }
}

/*
#[Swag.Mixin]
func(safetyWhat: string) safety(expr: code, message: string = null, loc = #callerlocation)
{
    #if #safety(safetyWhat)
    {
        let #mix0 = #inject #self.expr
        if !#mix0:
            @panic(#self.message, #self.loc)
    }
}

#[Swag.Mixin]
func safetyBoundCheck(expr: code, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}
*/


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
