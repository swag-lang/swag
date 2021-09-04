struct AA
{
}

impl AA
{
    #[Swag.Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {
        var @alias0: s32
    }
}


func toto(x: AA)
{
    visit v: dref &x
    {
    }
}

#test
{
    var x: AA
    toto(x)
}