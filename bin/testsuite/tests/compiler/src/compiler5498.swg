#global testerror "Err0042"

func toto()->any
{
    return 5
}

const X = #run toto()
