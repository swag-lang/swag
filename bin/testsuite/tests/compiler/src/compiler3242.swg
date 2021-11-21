#global testerror

#[Swag.Safety("", true)]
#test
{
    var x: any = 1.0
    if !x
        @print("toto")
}