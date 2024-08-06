#global testerror "Err0197"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}