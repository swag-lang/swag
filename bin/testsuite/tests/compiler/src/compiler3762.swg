//#global skip

#[Swag.Optimize(.All, false)]
#test
{
    var e = 0
    let c = 100'u8
    e = e * 10 + (c - '0''u8)
}
