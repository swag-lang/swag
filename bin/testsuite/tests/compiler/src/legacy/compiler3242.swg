#global testerror "Err0530"

#[Swag.Safety("", "", true)]
#test
{
    var x: any = 1.0
    if !x do
        @print("toto")
}
