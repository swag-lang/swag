struct SALUTE
{
    s, t: f64
}

//#[swag.printbc]
func cc(st: *SALUTE)
{
    st.s = 55
    st.t = 55
}

//#[swag.printbc]
#test
{
    var st: SALUTE
    cc(&st)
}