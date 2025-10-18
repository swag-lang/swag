#global testerror "Err0500"

struct(T) ArrayPtr
{
    ptr: T
}

struct XXX
{
    x:   ArrayPtr'XXX
}

#assert(#sizeof(XXX) == #sizeof(*void))
