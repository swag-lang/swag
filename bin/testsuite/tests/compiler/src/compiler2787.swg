#global testerror "Err0741"

func myFunc(val: s32, x: func(s32)->s32)
{
    x(val)
}
