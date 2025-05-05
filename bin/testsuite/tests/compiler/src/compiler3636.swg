#global testerror "Err0515"

struct(T) ArrayPtr
{
    ptr: T
}

struct XXX
{
    x: ArrayPtr'XXX
}

#assert(#sizeof(XXX) == #sizeof(*void))