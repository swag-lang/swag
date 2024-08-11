
#global testerror "Err0475"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}