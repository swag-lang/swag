#global testerror "Err0049"

func toto()->any
{
    return 5
}

const X = #run toto()
