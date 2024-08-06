#global testerror "Err0599"

#[Swag.Safety("", true)]
#test
{
    var x: any = 1.0
    if !x do
        @print("toto")
}