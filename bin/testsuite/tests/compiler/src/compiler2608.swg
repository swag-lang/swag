struct AA
{

}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        #mixin stmt
    }
}

func toto() throw
{
}

#test
{
    var x: AA
    visit x
    {
        try toto()
    }
}