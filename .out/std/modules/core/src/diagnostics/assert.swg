#global public
#global namespace Debug

#[Swag.inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr
        {
            @panic(message, loc)
        }
    }
}
