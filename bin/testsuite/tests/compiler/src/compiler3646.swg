interface I
{
    fct: func(self)
}

#[Swag.Safety("sanity", false)]
func toto()
{
    var x: [2] I
    x[0].fct()
}