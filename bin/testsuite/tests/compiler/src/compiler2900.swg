struct AA
{

}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using const self, stmt: #block void)
    {
    }
}

func toto(x: AA)
{
    foreach x
    {

    }
}

#test
{
    var v: AA
    toto(v)
}