#global testerror "Err0349"

interface I
{
}

impl I for A
{
}

struct A
{
    name: string = "toto"
}

struct B
{
    using root:      A
    using root1:     A
}

//#assert(#typeof(B) == B)
