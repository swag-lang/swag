#[Swag.Macro]
public func toto57(x: string)
    check
    {
        if !@isconstexpr(x) do
            @compilererror("toto", #location)
        return true
    }
{
}
