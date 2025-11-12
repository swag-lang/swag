#global testerror "Err0346"
// swc-expected-error:*

#[Swag.Mixin]
func toto()
{
    var #uniq: s32 // swc-expected-error {{parser_err_invalid_compiler}}
    #uniq += 1 // swc-expected-error {{parser_err_invalid_compiler}}
}

#test
{
    toto()
}