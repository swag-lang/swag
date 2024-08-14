#global testerror "Err0596"

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
