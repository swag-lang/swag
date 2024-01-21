#global testerror "Err0578"

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