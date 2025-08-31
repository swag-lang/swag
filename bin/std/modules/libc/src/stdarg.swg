#global export

alias va_list = Swag.CVaList

#[Swag.Mixin]
{
    func va_start(vl: *va_list, dummy: any) => @cvastart(vl)
    func va_end(vl: *va_list)               => @cvaend(vl)
}
