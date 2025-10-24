#global testerror "Err0285"

func toto()->s32
{
    let a = 0
    for
    {
        if a do
            return 1
        break
    }
}
