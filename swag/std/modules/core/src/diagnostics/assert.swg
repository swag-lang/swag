#global public
#global namespace debug

#[swag.inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr
        {
            @assertmsg(message, loc)
        }
    }
}
