typealias HANDLE: *void

#[swag.foreign]
func GetStdHandle(nStdHandle: u32)->HANDLE;
#test
{
	GetStdHandle(cast(u32) -10)
}