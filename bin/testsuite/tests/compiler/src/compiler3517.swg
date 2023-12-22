#global testerror "Err0748"

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