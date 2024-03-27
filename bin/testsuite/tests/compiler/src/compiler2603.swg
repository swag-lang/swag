#global testerror "Err0509"
//#global testerror "Err0704"

#[Swag.Macro]
func(ptr: bool) opVisitToto(x: string, stmt: code)
{
    loop x do
        #mixin stmt
}
