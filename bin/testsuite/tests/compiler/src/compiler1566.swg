#global testerror "Err0686"

//#[Swag.PrintBc]
#test
{
    var x = 0
    var y = 0
    x = #raw #raw y // swc-expected-error {{parser_err_invalid_compiler}}
}