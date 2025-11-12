#global testerror "Err0686"

#test
{
    defer #titi @print("a") // swc-expected-error {{parser_err_invalid_compiler}}
}