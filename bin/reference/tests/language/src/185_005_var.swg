/**
A global variable can be tagged with `#[Swag.Tls]` to store it in the thread local storage (one copy per thread).
*/
#[Swag.Tls]
var G = 0

/**
A local variable can be tagged with `#[Swag.Global]` to make it global (aka `static` in C/C++).
*/
#test
{
    func toto()->s32
    {
        #[Swag.Global]
        var G1 = 0

        G1 += 1
        return G1
    }

    @assert(toto() == 1)
    @assert(toto() == 2)
    @assert(toto() == 3)
}

/**
A global variable can also be marked as `#[Swag.Compiler]`. That kind of variable will not be exported to the runtime and can only be used in compile time code.
*/
#[Swag.Compiler]
var G2 = 0

#run
{
    G2 += 5
}