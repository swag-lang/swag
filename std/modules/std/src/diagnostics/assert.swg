namespace std.diagnostics.debug
{
	#[swag.inline]
	public func assert(expr: bool, message: string = null)
	{
		#if std.ASSERT
		{
			if !expr
				assertMessage(message)
		}
	}
}