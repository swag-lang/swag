#[swag.constexpr]
func toto() => "tag"

#test
{
    if(#tag(toto()))
    {

    }
}