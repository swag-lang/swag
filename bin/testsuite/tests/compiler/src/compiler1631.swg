
#global testerror "Err1172"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}