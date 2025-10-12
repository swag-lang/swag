//#global skip
#[Swag.Mixin]
func(safetyWhat: Swag.SafetyWhat) safety(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(safetyWhat)
    {
        let #uniq0 = #inject(expr)
        if !#uniq0 do
            @panic(message, loc)
        @assert(#uniq0 == true)
    }
}

#[Swag.Mixin]
func safetyBoundCheck(expr: #code bool, message: string = null, loc = #callerlocation)
{
    safety'(Swag.SafetyWhat.BoundCheck)(expr, message, loc)
}

#[Swag.Inline]
func(G) toto()
{
    safetyBoundCheck(true)
}

#[Swag.Optimize(.All, false)]
#test
{
    toto's32()
}
