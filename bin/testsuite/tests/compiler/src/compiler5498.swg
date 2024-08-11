#global testerror "Err0019"

func toto()->any
{
    return 5
}

const X = #run toto()
