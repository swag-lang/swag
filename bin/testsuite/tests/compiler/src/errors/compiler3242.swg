#global testerror "Err0023"

#[Swag.Safety(.All, .All, true)]
#test
{
    var x: any = 1.0
    if !x do
        @print("toto")
}
