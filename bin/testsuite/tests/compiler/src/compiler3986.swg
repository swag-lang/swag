struct AA
{

}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using const me, stmt: #code void)
    {
    }
}

func toto(x: const &AA)
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