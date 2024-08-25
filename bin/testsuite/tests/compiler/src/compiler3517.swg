#global testerror "Err0482"

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