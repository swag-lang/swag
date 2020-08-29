#[swag.constexpr]
func toto() => "tag"

//#[swag.printbc]
#test
{
    //@print(#tagval(f64, "tag1"))

    if(#hastag(toto()))
    {
        @print("tag\n")
    }
}