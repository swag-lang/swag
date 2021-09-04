struct(T) AA
{

}

impl AA
{
    #[Swag.Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {

    }
}

func toto(x: AA)
{
    visit v: x
    {
    }
}

#test
{
    var x: AA's32
    x.toto()
}
