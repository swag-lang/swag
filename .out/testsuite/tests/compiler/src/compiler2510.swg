#global skip // no more relative pointers (18 july 2021)
//#global testerror

struct AA
{
    x: [..]~32 s32
}

//#[Swag.PrintBc]
#test
{
    v := @[10, 20]
    var st: AA
    st.x = v

    st1 := st
    @assert(st1.x[0] == 10)
    @assert(st1.x[1] == 20)
}