//#[swag.printbc]
struct(K, V) A
{
    x: K = 666
    y: V = true
}

struct(K, V) B
{
    in: *A'(K, V)
}

#test
{
    var st: B'(s32, bool)
    //@assert(st.in.x == 666)
    //@assert(st.in.y == true)
}
