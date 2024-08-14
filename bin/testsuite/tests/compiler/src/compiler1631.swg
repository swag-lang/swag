
#global testerror "Err0477"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}