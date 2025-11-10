#global testerror "s32 is bad !"

func(T) toto()
    where
    {
        if T != bool do
            @compilererror("s32 is bad !", #location(T))
        return true
    }
{
}

#test
{
    toto's32()
}
