#global testerror "Err0195"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}