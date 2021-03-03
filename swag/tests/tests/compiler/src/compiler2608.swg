struct AA
{

}

impl AA
{
    #[swag.macro]
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