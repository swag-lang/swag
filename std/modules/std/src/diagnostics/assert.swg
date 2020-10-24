public namespace std.debug;

#[swag.inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if std.ASSERT
    {
        if !expr
        {
            @print(loc.fileName)
            @assertmsg(message, loc)
        }
    }
}
