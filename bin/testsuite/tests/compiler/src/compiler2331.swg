struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
        #macro
        {
            #if `ptr {}
        }
    }
}


#test
{
    var v: AA
    visit v
    {

    }
}