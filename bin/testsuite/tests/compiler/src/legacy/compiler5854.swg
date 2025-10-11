#[Swag.Macro]
func assert(expr: #code bool, msg: #code string = "a")
{
    if !#inject(expr):
        @assert(false)
    @assert(#inject(msg) == "a")
}

#test
{
    assert(true)
}
