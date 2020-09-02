namespace std.diagnostics.debug
{
	#[swag.inline]
	public func assert(expr: bool, message: string = null, loc = #callerlocation)
	{
		#if std.ASSERT
		{
			if !expr
				assertMessage(message, loc)
		}
	}
}