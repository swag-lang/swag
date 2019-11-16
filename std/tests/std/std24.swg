#import "std"
using std
#assert latin1.isUpper(cast(u8) 'A')
#assert !latin1.isUpper(cast(u8) 'a')
#assert latin1.isLower(cast(u8) 'a')