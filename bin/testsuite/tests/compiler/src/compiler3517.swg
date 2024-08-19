#global testerror "Err0482"

func toto()->s32
{
    let a = 0
    loop
    {
        if a:
            return 1
        break
    }
}