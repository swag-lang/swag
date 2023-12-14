#global testerror "Err0067"
//#global testerror "Err0625"

#[Swag.Macro]
func(ptr: bool) opVisit,toto(x: string, stmt: code)
{
    loop x #mixin stmt
}
/*
#test
{
    visit(Toto) "toto"
    {

    }
}*/