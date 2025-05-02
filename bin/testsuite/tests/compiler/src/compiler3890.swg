#global testerror "Err0004"
struct A {}
impl A {
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        #macro
        {
            var #alias0: s32
            #inject #up stmt
        }
    }
}

#test
{
    var x = 0

    var a: A
    foreach x in a
    {
    }
}