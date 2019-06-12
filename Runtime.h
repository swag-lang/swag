const char* g_Runtime = R"(

namespace swag {

attr ConstExpr: () -> func;
attr PrintByteCode: () -> func;
attr Test: () -> func;
attr Compiler: () -> func;

func @print: (value: s32);
func @print: (value: s64);
func @print: (value: f32);
func @print: (value: f64);
func @print: (value: char);
func @print: (value: string);
func @assert: (value: bool);

}
)";
