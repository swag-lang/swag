#[Swag.NoReturn, Swag.macro]
func toto()
{
    #[Swag.Safety("", true)]
    #macro
    {
        return
    }
}

#test
{
    toto()
    @assert(false)
}