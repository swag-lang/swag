
#global testerror "Err0485"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}