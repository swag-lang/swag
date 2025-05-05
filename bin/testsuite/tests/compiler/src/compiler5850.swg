#[Swag.Macro]
func assert(expr: #code bool)
{
    if !#inject(expr):
        @assert(false)
}

#test
{
    assert(true)
}