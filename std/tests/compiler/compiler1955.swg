#[swag.noreturn, swag.inline]
func toto()
{
    return
}

#test
{
    toto()
    @assert(false)
}