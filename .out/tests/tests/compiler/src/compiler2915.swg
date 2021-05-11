var ppp = 0
func decode()->s32 throw
{
    return 0
}

func toto() throw
{
    ppp += 1
    while true
    {
        value := try decode()
        if value < 0 continue
        if value == 0 break
    }
}