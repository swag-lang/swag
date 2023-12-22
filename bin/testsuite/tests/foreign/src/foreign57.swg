#[Swag.Macro]
public func toto57(x: string)
    #validifx
    {
        if !@isconstexpr(x) do
            @compilererror("toto", #location)
        return true
    }
{
}
