struct A
{
    v: [2, 2] bool = true
}

//#global testerror
//#[Swag.PrintBc]
#test
{
    var a: A
    @assert(a.v[0, 0] == true)
    @assert(a.v[0, 1] == true)
    @assert(a.v[1, 0] == true)
    @assert(a.v[1, 1] == true)
}
