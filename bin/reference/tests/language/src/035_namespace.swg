// You can create a global scope with a namespace
// All symbols inside the namespace will be in the corresponding global scope.
namespace A
{
    func a() => 1
}

// You can also specify more than one name
// Here 'C' will be a namespace inside 'B' which is itself inside 'A'
namespace A.B.C
{
    func a() => 2
}

#test
{
    @assert(A.a() == 1)
    @assert(A.B.C.a() == 2)
}