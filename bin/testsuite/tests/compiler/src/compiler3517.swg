#global testerror "Err0510"

func toto()->s32
{
    let a = 0
    for
    {
        if a:
            return 1
        break
    }
}