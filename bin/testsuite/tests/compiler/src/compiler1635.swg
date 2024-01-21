#global testerror "Err0692"

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