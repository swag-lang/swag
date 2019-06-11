const char* g_Runtime = R"(

namespace swag {

attr constExpr: () -> func;
attr printByteCode: () -> func;
attr test: () -> func;

func @print: (value: s32);
func @print: (value: s64);
func @print: (value: f32);
func @print: (value: f64);
func @print: (value: char);
func @print: (value: string);
func @assert: (value: bool);

}
)";
