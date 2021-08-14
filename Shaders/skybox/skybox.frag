#version 450 core

layout (location = 0) out vec4 tDiffuse;
layout (location = 1) out vec4 tMasks;

layout (location = 0) in vec2 _uv;
layout (location = 1) in vec3 _pos;

layout(set = 1, binding = 1) uniform sampler2D back;
layout(set = 1, binding = 2) uniform sampler2D front;
layout(set = 1, binding = 3) uniform sampler2D left;
layout(set = 1, binding = 4) uniform sampler2D right;
layout(set = 1, binding = 5) uniform sampler2D bottom;
layout(set = 1, binding = 6) uniform sampler2D top;

layout (std140, set = 1, binding = 0) uniform MaterialData{
    bool hasBack;
	bool hasFront;
	bool hasLeft;
	bool hasRight;
	bool hasBottom;
	bool hasTop;

	vec3 tint_color;
	float Exposure;
};

int convert_xyz_to_cube_index(float x, float y, float z)
{
	int index;
	
  float absX = abs(x);
  float absY = abs(y);
  float absZ = abs(z);
  
  bool isXPositive = x > 0 ? true : false;
  bool isYPositive = y > 0 ? true : false;
  bool isZPositive = z > 0 ? true : false;
  
  // POSITIVE X
  if (isXPositive && (absX >= absY) && (absX >= absZ)) {
    index = 0;
  }
  // NEGATIVE X
  if (!isXPositive && absX >= absY && absX >= absZ) {
    index = 1;
  }
  // POSITIVE Y
  if (isYPositive && absY >= absX && absY >= absZ) {
    index = 2;
  }
  // NEGATIVE Y
  if (!isYPositive && absY >= absX && absY >= absZ) {
    index = 3;
  }
  // POSITIVE Z
  if (isZPositive && absZ >= absX && absZ >= absY) {
    index = 4;
  }
  // NEGATIVE Z
  if (!isZPositive && absZ >= absX && absZ >= absY) {
    index = 5;
  }

  return index;
}

vec2 convert_xyz_to_cube_uv(vec3 dir)
{
  float absX = abs(dir.x);
  float absY = abs(dir.y);
  float absZ = abs(dir.z);
  
	float x = dir.x;
	float y = dir.y;
	float z = dir.z;

  bool isXPositive = dir.x > 0 ? true : false;
  bool isYPositive = dir.y > 0 ? true : false;
  bool isZPositive = dir.z > 0 ? true : false;
  
  float maxAxis, uc, vc;
  
  // POSITIVE X
  if (isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from +z to -z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = -z;
    vc = y;
  }
  // NEGATIVE X
  if (!isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from -z to +z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = z;
    vc = y;
  }
  // POSITIVE Y
  if (isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from +z to -z
    maxAxis = absY;
    uc = x;
    vc = -z;
  }
  // NEGATIVE Y
  if (!isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -z to +z
    maxAxis = absY;
    uc = x;
    vc = z;
  }
  // POSITIVE Z
  if (isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = x;
    vc = y;
  }
  // NEGATIVE Z
  if (!isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from +x to -x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = -x;
    vc = y;
  }

  // Convert range from -1 to 1 to 0 to 1
  float u = 0.5f * (uc / maxAxis + 1.0f);
  float v = 0.5f * (vc / maxAxis + 1.0f);

	return vec2(u, v);
}

vec3 sample_sky(int index, vec2 uv){
	vec2 my = uv;
	my.y *= -1;
    switch (index)
    {
        case 0:
        {
            return texture(right, my).xyz;
        }
        case 1:
        {
            return texture(left, my).xyz;
        }
        case 3:
        {
            return texture(bottom, my).xyz;
        }
        case 2:
        {
            return texture(top, my).xyz;
        }
        case 5:
        {
            return texture(back, my).xyz;
        }
        case 4:
        {
            return texture(front, my).xyz;
        }
    }
}

void main()
{   
	int index = convert_xyz_to_cube_index(_pos.x, _pos.y, _pos.z);
  vec2 uv = convert_xyz_to_cube_uv(vec3(_pos.x, _pos.y, _pos.z));
	vec3 color = sample_sky(index, uv);
	color *= tint_color;
	color *= Exposure;

	tDiffuse.xyz = color;
	tMasks = vec4(0, 0, 0, 0);
}
