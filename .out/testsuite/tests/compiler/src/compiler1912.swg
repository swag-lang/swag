#[Swag.ConstExpr]
func toto() => "tag"

//#[Swag.printbc]
#test
{
    //@print(#tagval(f64, "tag1"))

    if(#hastag(toto()))
    {
        @print("tag\n")
    }
}