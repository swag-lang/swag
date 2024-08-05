#global testerror "Err0236"

func toto()->{x: s32} 
{
    return {0}
}

#test
{
    if toto() <= toto()
    {

    }
}
