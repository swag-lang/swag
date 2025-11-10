#[Swag.Overload]
func fatal(x: string, args: ...) {}
#[Swag.Overload]
func fatal() {}

#test
{
    fatal()
}
