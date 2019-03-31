#version 450 core

#define PI 3.141592653589793

out vec3 out_Color;

layout (binding = 0) uniform sampler2D u_ViewSpacePosition;
layout (binding = 1) uniform sampler2D u_ViewSpaceNormal;

uniform struct Material
{
	vec4 albedo;
	float reflectivity;
	float shininess;
	float metalness;
	float fresnel;
} u_Material;

struct Light
{
	vec4 viewSpacePosition;
	vec4 color;
};

layout (std430, binding = 3) buffer LightBuffer
{
	Light lights[];
};


float F(float wi_wh)
{
	return u_Material.fresnel + (1.0 - u_Material.fresnel) * pow(1.0 - wi_wh, 5.0);
}

float D(float n_wh)
{
	return (u_Material.shininess + 2.0) * (1.0 / (2.0 * PI)) * pow(n_wh, u_Material.shininess);
}

float G(float n_wi, float n_wo, float n_wh, float wo_wh)
{
	float coeff = 2.0 * n_wh / wo_wh;
	return min(1.0, min(coeff * n_wo, coeff * n_wi));
}

float brdf(float F, float D, float G, float n_wo, float n_wi)
{
	return F * D * G / (4.0 * n_wi * n_wo);
}


void main()
{
	ivec2 texCoords = ivec2(gl_FragCoord.xy);

	vec3 viewSpacePosition = texelFetch(u_ViewSpacePosition, texCoords, 0).rgb;
	vec3 n = texelFetch(u_ViewSpaceNormal, texCoords, 0).rgb;

	uint numLights = lights.length();

	for (int i = 0; i < numLights; i++)
	{
		vec3 wi = lights[i].viewSpacePosition.xyz - viewSpacePosition;
		float inv_d2 = 1.0 / dot(wi, wi);
		wi = normalize(wi);
		float n_wi = dot(n, wi);
		if (n_wi <= 0.0)
			continue;

		vec3 Li = lights[i].color.rgb * inv_d2;
		vec3 wo = -normalize(viewSpacePosition);
		vec3 wh = normalize(wi + wo);

		float n_wo = dot(n, wo);
		float n_wh = dot(n, wh);
		float wi_wh = dot(wi, wh);
		float wo_wh = dot(wo, wh);

		float F = F(wi_wh);
		float D = D(n_wh);
		float G = G(n_wi, n_wo, n_wh, wo_wh);

		float brdf = brdf(F, D, G, n_wo, n_wi);

		vec3 diffuse_term = u_Material.albedo.rgb * (1.0 / PI) * n_wi * Li;
		vec3 dielectricTerm = brdf * n_wi * Li + (1.0 - F) * diffuse_term;
		vec3 metalTerm = brdf * u_Material.albedo.rgb * n_wi * Li;
		vec3 microfacetTerm = u_Material.metalness * metalTerm + (1.0 - u_Material.metalness) * dielectricTerm;

		out_Color += u_Material.reflectivity * microfacetTerm + (1.0 - u_Material.reflectivity) * diffuse_term;
	}
}