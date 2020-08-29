#[swag.constexpr]
func toto() => "tag"

#test
{
    if(#tag("tag1"))
    {
        @print("tag1\n")
    }

    if(#tag(toto()))
    {
        @print("tag\n")
    }
}