#global testerror "Err0230"

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
