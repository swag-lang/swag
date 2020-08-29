#[swag.constexpr]
func toto() => "tag"

#test
{
    @print(#tag("tag1"))

    if(#tag(toto()))
    {
        @print("tag\n")
    }
}