#global testerror "Err0017"

func toto()->any
{
    return 5
}

const X = #run toto()
