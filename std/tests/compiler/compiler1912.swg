#[swag.constexpr]
func toto() => "tag"

#test
{
    @print(#tagval(f64, "tag1"))

    if(#hastag(toto()))
    {
        @print("tag\n")
    }
}