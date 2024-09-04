#global testerror "Err0032"

func toto()->any
{
    return 5
}

const X = #run toto()
