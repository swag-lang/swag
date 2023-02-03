#[Swag.Macro]
public func toto57(x: string)
    #validifx
    {
        if !@isconstexpr(x)
            @compilererror("toto", #location)
        return true
    }
{
}
