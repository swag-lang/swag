
#global testerror "Err0470"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}