#[Swag.Macro]
func assert(expr: #code bool, msg: #code string = "a")
{
    if !#inject(expr) do
        @assert(false)
    @assert(#inject(msg) == "true")
}

#test
{
    assert(true, "true")
}
