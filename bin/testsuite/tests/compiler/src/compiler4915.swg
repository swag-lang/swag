#global testerror "Err0588"

#[Swag.Mixin]
func toto(stmt: #code void)
{
    var #alias33 = 0 // swc-expected-error {{parser_err_invalid_compiler}}
}