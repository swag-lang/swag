#[Swag.Macro]
public func toto57(x: string)
    verify
    {
        if !#isconstexpr(x):
            @compilererror("toto", #curlocation)
        return true
    }
{
}
