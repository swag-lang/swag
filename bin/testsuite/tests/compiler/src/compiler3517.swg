#global testerror "Err0575"

func toto()->s32
{
    let a = 0
    loop
    {
        if a do
            return 1
        break
    }
}