#global testerror "is invalid"
struct B1666
{
}

struct(T) A
{
}
impl A
{
    #[Swag.Inline]
    mtd toto()
    {
        #error "type [[" ++ @typeof(T).name ++ "]] is invalid"
    }
}

#test
{
    var x: A'B1666
    x.toto()
}
