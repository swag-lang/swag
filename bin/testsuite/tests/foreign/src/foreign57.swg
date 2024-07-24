#[Swag.Macro]
public func toto57(x: string)
    where,call
    {
        if !@isconstexpr(x) do
            @compilererror("toto", #location)
        return true
    }
{
}
