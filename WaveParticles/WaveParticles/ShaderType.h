#pragma once

namespace Core::Shaders
{
	// typical usage
	typedef enum
	{
		SHADER_TYPE_VF,
		SHADER_TYPE_VGF
	} ShaderType;

	// for transform feedback
	typedef enum
	{
		SHADER_TYPE_V,
		SHADER_TYPE_VG,
	} TransformFeedbackShaderType;
}
