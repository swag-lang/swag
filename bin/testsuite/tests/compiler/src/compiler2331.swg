struct AA
{
    count: s64 = 666
}

impl AA
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(using self, stmt: code)
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
    visit v
    {

    }
}