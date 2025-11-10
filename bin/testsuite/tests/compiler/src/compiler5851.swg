#[Swag.Macro]
func assert(expr: #code bool, msg: #code string)
{
    if !#inject(expr) do
        @assert(false)
}

#test
{
    assert(true, "true")
}
