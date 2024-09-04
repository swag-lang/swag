#global testerror "Err0464"

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