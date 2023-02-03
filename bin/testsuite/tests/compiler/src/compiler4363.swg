#global testerror

func(T) toto()
    #selectif
    {
        if T != bool
            @compilererror("s32 is bad !", @location(T))
        return true
    }
{
}

#test
{
    toto's32()
}