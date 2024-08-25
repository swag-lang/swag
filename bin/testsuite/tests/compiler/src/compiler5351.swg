//#global skip
struct String {}

impl String
{
    #[Swag.Macro]
    mtd(a: bool, b: bool) opVisit(s: code)
    {
        #macro
        {
            #mixin #up s
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