#[swag.constexpr]
{
	func main () -> bool { return true; }
	func main1 () -> bool { return false; }
}

#assert main()
#assert !main1()
#assert main() && true
#assert main1() || true
