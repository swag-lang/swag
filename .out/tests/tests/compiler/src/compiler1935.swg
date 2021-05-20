struct SALUTE
{
    s, t: f64
}

//#[Swag.printbc]
func cc(st: *SALUTE)
{
    st.s = 55
    st.t = 55
}

//#[Swag.printbc]
#test
{
    var st: SALUTE
    cc(&st)
}