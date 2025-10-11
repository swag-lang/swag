struct S {}

impl S
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisitBytes(stmt: #code void)
    {
        for 4
        {
            let #alias0 = @index
            #inject(stmt)
        }
    }

    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        me.opVisitBytes'(ptr, back)(|#alias0| stmt)
    }
}

#test
{
    var s: S
    foreach c in s
    {
    }

    foreach #Bytes #reverse c in s
    {
    }
}
