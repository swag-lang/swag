#global testerror "Err0686"
#test
{
    foreach #0 x: toto // swc-expected-error {{parser_err_invalid_compiler}}
    {
    }
}