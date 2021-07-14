#[Swag.NoReturn, Swag.macro]
func toto()
{
    #[Swag.safety("", true)]
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