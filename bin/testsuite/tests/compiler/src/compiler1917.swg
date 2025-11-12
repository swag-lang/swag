#global testerror "Err0686"

struct T
{

}

public #bake C1914s32 = T // swc-expected-error {{parser_err_invalid_compiler}}
