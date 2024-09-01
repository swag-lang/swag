#[Swag.Macro]
public func toto57(x: string)
    where<call>
    {
        if !#isconstexpr(x):
            @compilererror("toto", #curlocation)
        return true
    }
{
}
