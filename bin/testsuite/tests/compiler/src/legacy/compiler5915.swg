namespace A5915
{
    func toto() {}
}

#[Swag.Using(A5915)]
func titi()
{
    func tata()
    {
        toto()
    }
}
