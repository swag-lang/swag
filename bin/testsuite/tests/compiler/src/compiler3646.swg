interface I
{
    func fct(self);
}

#[Swag.Safety("sanity", false)]
func toto()
{
    var x: [2] I
    x[0].fct()
}