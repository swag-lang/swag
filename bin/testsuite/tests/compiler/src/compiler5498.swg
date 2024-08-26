#global testerror "Err0036"

func toto()->any
{
    return 5
}

const X = #run toto()
