#[swag.noreturn, swag.macro]
func toto()
{
    #[swag.safety(true)]
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