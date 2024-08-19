#global testerror "Err0038"

#[Swag.Safety("", true)]
func toto(a: s32)
{
    let b = -1
    let c = a / b
    @print(c)
}

#test
{
    let x = Swag.S8.Min / -1
    #print @typeof(x).name
    //toto(Swag.S32.Min)
}
