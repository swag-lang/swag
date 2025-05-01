struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code void)
    {
        #macro
        {
            #if #up ptr {}
        }
    }
}


#test
{
    var v: AA
    foreach v
    {

    }
}