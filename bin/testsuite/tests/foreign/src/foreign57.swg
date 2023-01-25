#[Swag.Macro]
public func toto57(x: string)
    #selectif
    {
        if !@isconstexpr(x)
            @compilererror("toto", #location)
        return true
    }
{
}
