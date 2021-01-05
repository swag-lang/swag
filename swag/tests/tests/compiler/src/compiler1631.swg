
#global testerror

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}