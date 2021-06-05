struct AA
{

}

impl AA
{
    #[Swag.macro]
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