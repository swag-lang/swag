
#global testerror "assertion failed"

internal struct A
{
    x: u32
    y: u32
}

#run
{
    var st: A
    if st.y == 0 do
        @assert(false)
}