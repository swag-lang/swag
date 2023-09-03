#global testerror "s32 is bad !"

func(T) toto()
    #validif
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