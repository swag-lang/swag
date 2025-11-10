#[Swag.Macro]
func debugExpr(expr: #code any, res: string)
{
    let str = "[DEBUG] " ++ #stringof(expr) ++ " = " ++ #inject(expr)
    @assert(str == res)
}

#test
{
    const x = 5
    debugExpr(x * 2, "[DEBUG] x * 2 = 10")
    debugExpr(2 + 3 * 4, "[DEBUG] 2 + 3 * 4 = 14")
}
