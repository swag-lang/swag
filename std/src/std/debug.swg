namespace std
{
	#[swag.public]
	func assert(expr: bool)
	{
		@assert(expr)
	}
}