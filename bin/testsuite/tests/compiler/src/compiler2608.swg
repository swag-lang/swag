struct AA
{

}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using me, stmt: #code void)
    {
        #inject(stmt)
    }
}

func toto() throw
{
}

#test
{
    var x: AA
    foreach x
    {
        try toto()
    }
}