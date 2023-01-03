#pragma once

#include <glslang/Include/glslang_c_interface.h>
#include "../ShaderStages.hpp"


bool CompileFromGLSL(const char* shader_data, ShaderStagesBits stage, unsigned char** out, unsigned int& spv_size);