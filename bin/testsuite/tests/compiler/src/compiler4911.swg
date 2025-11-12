#global testerror "Err0344"

#[Swag.Mixin]
func toto()
{
    var #alias: s32 // swc-expected-error {{parser_err_invalid_compiler}}
    #alias += 1 // swc-expected-error {{parser_err_invalid_compiler}}
}

#test
{
    toto()
}