#global testerror "Err0686"

#test
{
    if(#tag(6)) // swc-expected-error {{parser_err_invalid_compiler}}
    {

    }
}