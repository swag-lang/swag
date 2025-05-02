#[Swag.Macro]
func assert(expr: #block bool)
{
    if !#inject expr:
        @assert(false)
}

#test
{
    assert(true)
}