#shader vertex
#version 330

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out vec2 v_TexCoord;
out vec3 v_NormalLocal;
out vec3 v_NormalWorld;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_MVP *  vec4(position.x, position.y, position.z, 1.0);
	v_TexCoord = texCoord;
	v_NormalLocal = normal;
	v_NormalWorld = normalize(mat3(u_Model) * normal);
}

#shader fragment
#version 330

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 v_NormalLocal;
in vec3 v_NormalWorld;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform int u_Picking;
uniform vec3 u_FaceColors[6];

void main()
{
	if (u_Picking == 1)
	{
		FragColor = u_Color;
		return;
	}
	vec3 n = normalize(v_NormalLocal);
	vec3 faceColor = u_FaceColors[0];
	if (n.x > 0.5)
	{
		faceColor = u_FaceColors[0];
	}
	else if (n.x < -0.5)
	{
		faceColor = u_FaceColors[1];
	}
	else if (n.y > 0.5)
	{
		faceColor = u_FaceColors[2];
	}
	else if (n.y < -0.5)
	{
		faceColor = u_FaceColors[3];
	}
	else if (n.z > 0.5)
	{
		faceColor = u_FaceColors[4];
	}
	else if (n.z < -0.5)
	{
		faceColor = u_FaceColors[5];
	}

	vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
	vec3 finalColor = texColor * faceColor;
	FragColor = vec4(finalColor, 1.0);
}
