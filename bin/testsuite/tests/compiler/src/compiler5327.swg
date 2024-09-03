struct S {}

impl S
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisitBytes(stmt: code)
    {
        for 4
        {
            let #alias0 = #index
            #inject stmt
        }
    }

    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        opVisitBytes'(ptr, back)(|#alias0| stmt)
    }
}

#test
{
    var s: S
    foreach c in s
    {
    }

    foreach<Bytes> #back c in s
    {
    }
}