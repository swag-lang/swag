#import "std"
using std

#assert latin1.isUpper(cast(u8) "A"'char)
#assert !latin1.isUpper(cast(u8) "a"'char)
#assert latin1.isLower(cast(u8) "a"'char)