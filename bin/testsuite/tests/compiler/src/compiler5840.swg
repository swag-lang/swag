//#global skip
#[Swag.Mixin]
func(safetyWhat: string) safety(expr: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(safetyWhat)
    {
        let #uniq0 = #inject(#self.expr)
        if !#uniq0:
            @panic(#self.message, #self.loc)
        @assert(#uniq0 == true)
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