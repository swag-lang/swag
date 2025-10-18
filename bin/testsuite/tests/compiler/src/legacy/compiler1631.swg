
#global testerror "Err0587"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}