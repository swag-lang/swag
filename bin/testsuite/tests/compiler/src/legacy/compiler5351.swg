//#global skip
struct String {}

impl String
{
    #[Swag.Macro]
    mtd(a: bool, b: bool) opVisit(s: #code void)
    {
        #macro
        {
            #inject(#up s)
        }
    }
}

#test
{
    var a: String
    foreach a
    {
        foreach a
        {
            foreach a
            {
            }
        }
    }
}
