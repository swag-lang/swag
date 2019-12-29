namespace std.diagnostics.debug
{
	#[swag.inline]
	public func assert(expr: bool, message: string = null)
	{
		#if swag.ASSERT
		{
			if !expr
				os.assertMessage(message)
		}
	}
}