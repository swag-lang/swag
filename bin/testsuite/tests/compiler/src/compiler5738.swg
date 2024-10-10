#global testerror "Err0675"
struct A
{
    enum Kind
    {
        X, Y
    }

    kind: Kind
}

#test
{
    let a = A{kind: .Z}
}