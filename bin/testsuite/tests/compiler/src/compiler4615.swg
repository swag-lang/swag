//#global testerror "Err0821"
#[Swag.Safety(.All, .Math, false)]
#test
{
    const y = -128's8
    let x   = @abs(y)
    //@print(x)
}
