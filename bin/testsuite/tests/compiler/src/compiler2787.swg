#global testerror "Err0702"

func myFunc(val: s32, x: func(s32)->s32)
{
    x(val)
}
