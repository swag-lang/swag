
;
using swag;

#unittest error;
func main00 () { var b: f32 = 0; b <<= 1; }
#unittest error;
func main01 () { var b: f64 = 0; b <<= 1; }
#unittest error;
func main02 () { var b: f32 = 0; b >>= 1; }
#unittest error;
func main03 () { var b: f64 = 0; b >>= 1; }
#unittest error;
func main04 () { var b: f32 = 0; b ^= 1; }
#unittest error;
func main05 () { var b: f64 = 0; b |= 1; }
#unittest error;
func main06 () { var b: f64 = 0; b &= 1; }
