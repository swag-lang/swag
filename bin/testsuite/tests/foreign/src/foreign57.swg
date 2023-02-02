#[Swag.Macro]
public func toto57(x: string)
    #selectifx
    {
        if !@isconstexpr(x)
            @compilererror("toto", #location)
        return true
    }
{
}
