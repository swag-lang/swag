interface I
{
    func fct(me);
}

#[Swag.Sanity(false)]
func toto()
{
    var x: [2] I
    x[0].fct()
}
