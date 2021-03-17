namespace tt
{
    #[swag.macro]
    func(ptr: bool) opVisitToto(x: string, stmt: code)
    {
        loop x #mixin stmt
    }
}

#test
{
    {
        using tt
        visit(Toto) "toto"
        {

        }
    }
}