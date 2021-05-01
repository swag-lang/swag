#[swag.constexpr]
{
	func main () -> bool { return true; }
	func main1 () -> bool { return false; }
}

#assert main()
#assert !main1()
#assert main() and true
#assert main1() or true
