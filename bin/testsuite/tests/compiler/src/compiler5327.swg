struct S {}

impl S
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit,bytes(stmt: code)
    {
        loop 4
        {
            let #alias0 = #index
            #mixin stmt
        }
    }

    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        opVisitbytes'(ptr, back)(|#alias0| stmt)
    }
}

#test
{
    var s: S
    visit c: s
    {
    }

    visit:bytes c: s
    {
    }
}