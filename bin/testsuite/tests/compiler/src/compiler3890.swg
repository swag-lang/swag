#global testerror
struct A {}
impl A {
    #[Swag.Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {
        #macro
        {
            var @alias0: s32
            #mixin stmt
        }
    }
}

#test
{
    x := 0

    var a: A
    visit x: a
    {
    }
}