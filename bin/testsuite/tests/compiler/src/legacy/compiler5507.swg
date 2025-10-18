#global testerror "Err0616"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}
