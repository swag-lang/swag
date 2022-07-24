//#global skip

#[Swag.Optim("", false)]
#test
{
    e := 0
    c := 100'u8
    e = e*10 + (c - "0"'u8)
}