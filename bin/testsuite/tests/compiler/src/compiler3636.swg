#global testerror "Err0535"

struct(T) ArrayPtr
{
    ptr: T
}

struct XXX
{
    x: ArrayPtr'XXX
}

#assert @sizeof(XXX) == @sizeof(*void)