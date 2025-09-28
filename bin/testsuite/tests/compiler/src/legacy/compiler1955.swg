#[Swag.CalleeReturn, Swag.Macro]
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
