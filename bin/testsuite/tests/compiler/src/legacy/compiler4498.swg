#global testerror "executing unreachable code"
#[Swag.Safety(.All, "", true)]
#test
{
    unreachable
}
