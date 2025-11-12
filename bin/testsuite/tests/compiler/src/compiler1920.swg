#global testerror "Err0686"

struct(T) C1920
{

}

#bake C1920 = C1920's32 // swc-expected-error {{parser_err_invalid_compiler}}
