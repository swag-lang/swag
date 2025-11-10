#[Swag.Macro]
func assert(expr: #code bool, msg: #code string = null)
{
    if !#inject(expr) do
        @assert(false)
}

#test
{
    assert(true, "true")
}
