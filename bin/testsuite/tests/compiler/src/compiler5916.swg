namespace A5916
{
    func toto() {}
}

#[Swag.Using(A5916), Swag.Inline]
func titi()
{
    func tata()
    {
        toto()
    }
}

#test
{
    titi()
}
