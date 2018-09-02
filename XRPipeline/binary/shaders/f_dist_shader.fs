#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec2 TexCordsNormalized  = 2.0 * TexCoords - 1.0;
	float r = length(TexCordsNormalized);
	vec2 TexCordsST;
	float K0 = 1, K2 = 0.22, K4 = 0.25;
	float r2 = r * r; 
	float r4 = r2 * r2;
	float Scale = (K0 + K2 * r2 + K4 * r4);
	vec2 TexCordsDist = TexCordsNormalized * Scale;
	TexCordsST = (1.0 + TexCordsDist) * 0.5;
    vec3 col = texture(screenTexture, TexCordsST).rgb;
    FragColor = vec4(col, 1.0);
} 