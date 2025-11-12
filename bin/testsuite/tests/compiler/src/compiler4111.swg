#global testerror "Err0722"
#test
{
    @asssert(false) // swc-expected-error {{parser_err_invalid_intrinsic}}
}