typealias HANDLE: u32

#[swag.foreign]
func GetStdHandle(nStdHandle: u32)->HANDLE;
#test
{
	GetStdHandle(cast(u32) -10)
}