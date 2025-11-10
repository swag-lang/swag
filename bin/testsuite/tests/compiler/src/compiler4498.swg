#global testerror "executing unreachable code"
#[Swag.Safety(.All, .All, true)]
#test
{
    unreachable
}
