struct AA
{

}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool) opVisit(using const self, stmt: code)
    {
    }
}

func toto(x: AA)
{
    visit x
    {

    }
}

#test
{
    var v: AA
    toto(v)
}