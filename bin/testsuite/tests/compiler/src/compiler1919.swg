#global testerror "Err0686"

struct(T) A
{

}

#bake C1914s32 = A's32 // swc-expected-error {{parser_err_invalid_compiler}}
#bake C1914s321 = A's32 // swc-expected-error {{parser_err_invalid_compiler}}
