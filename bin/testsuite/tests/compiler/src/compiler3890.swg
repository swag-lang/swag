#global testerror "Err0511"
struct A {}
impl A {
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        #macro
        {
            var #alias0: s32
            #mixin #up stmt
        }
    }
}

#test
{
    var x = 0

    var a: A
    visit x in a
    {
    }
}