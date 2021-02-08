#[swag.printbc]
#test
{
    x := "toto"
    if x == null
    {
        @assert(false)
    }
}