const char* g_Runtime = R"(

namespace swag {

attr constexpr() -> func;
attr printbytecode() -> func;
attr compiler() -> func;
attr public() -> func;
attr foreign(module: string = "") -> func;
}

func @print(value: s32);
func @print(value: s64);
func @print(value: f32);
func @print(value: f64);
func @print(value: char);
func @print(value: string);
func @assert(value: bool);

)";
