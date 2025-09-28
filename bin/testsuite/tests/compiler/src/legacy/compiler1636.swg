#global testerror "Err0645"

readonly struct A
{
    x: u32
    y: u32
}

#run
{
    var st: A
    st.y = 0
}