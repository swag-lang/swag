#global testerror
struct B1666 {}

struct(T) A {}
impl A
{
    #[Swag.Inline]
    mtd toto()
    {
        #assert(false, "type '" ++ @typeof(T).name ++ "' is invalid")
    }
}

#test
{
    var x: A'B1666
    x.toto()
}