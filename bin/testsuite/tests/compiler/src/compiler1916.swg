#global testerror "Err0686"

func toto()
{
}

alias T = s32
public #bake C1914s32 = T // swc-expected-error {{parser_err_invalid_compiler}}
