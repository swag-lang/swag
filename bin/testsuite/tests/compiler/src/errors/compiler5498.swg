#global testerror "Err0048"

func toto()->any
{
    return 5
}

const X = #run toto()
