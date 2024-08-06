#global testerror "Err0248"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}