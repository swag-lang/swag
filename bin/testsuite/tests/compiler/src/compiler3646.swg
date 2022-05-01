interface I
{
    fct: func(self)
}

func toto()
{
    var x: [2] I
    x[0].fct()
}