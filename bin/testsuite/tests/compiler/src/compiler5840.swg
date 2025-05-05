//#global skip
#[Swag.Mixin]
func(safetyWhat: string) safety(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(safetyWhat)
    {
        let #mix0 = #inject(#self.expr)
        if !#mix0:
            @panic(#self.message, #self.loc)
        @assert(#mix0 == true)
    }
}

#[Swag.Mixin]
func safetyBoundCheck(expr: #code bool, message: string = null, loc = #callerlocation)
{
    safety'("boundcheck")(expr, message, loc)
}

#[Swag.Inline]
func(G) toto()
{
    safetyBoundCheck(true)
}

#[Swag.Optimize("", false)]
#test
{
    toto's32()
}