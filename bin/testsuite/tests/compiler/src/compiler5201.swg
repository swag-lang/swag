#global testerror "Err0591"

struct A 
{ 
    str: string 
    x:   s32
}

struct B 
{ 
    using a: A
    y: bool
}

#test
{
    let b = B{"str", true}
    @assert(b.str == "str")
}