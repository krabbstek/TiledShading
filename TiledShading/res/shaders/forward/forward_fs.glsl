#version 430 core

#define PI 3.141592653589793

in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

out vec3 out_Color;

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
	vec3 n = normalize(viewSpaceNormal);
	vec3 wo = -normalize(viewSpacePosition);
	float n_wo = dot(n, wo);

	vec3 totalDiffuseTerm = vec3(0.0);
	vec3 dielectricTerm = vec3(0.0);
	vec3 metalTerm = vec3(0.0);
	vec3 diffuseTermPreComp = u_Material.albedo.rgb * (1.0 / PI);

	uint numLights = lights.length();

	for (int i = 0; i < numLights; i++)
	{
		vec3 wi = lights[i].viewSpacePosition.xyz - viewSpacePosition;
		float lightThreshold = lights[i].color.a;
		float inv_d2 = max(1.0 / dot(wi, wi) - lightThreshold, 0.0);
		wi = normalize(wi);
		float n_wi = dot(n, wi);
		if (n_wi <= 0.0)
			continue;

		vec3 Li = lights[i].color.rgb * inv_d2;
		vec3 wh = normalize(wi + wo);

		float n_wh = dot(n, wh);
		float wi_wh = dot(wi, wh);
		float wo_wh = dot(wo, wh);

		float F = F(wi_wh);
		float D = D(n_wh);
		float G = G(n_wi, n_wo, n_wh, wo_wh);

		float brdf = brdf(F, D, G, n_wo, n_wi);

		vec3 n_wi_Li = n_wi * Li;
		vec3 diffuseTerm = diffuseTermPreComp * n_wi_Li;
		dielectricTerm += brdf * n_wi * Li + (1.0 - F) * diffuseTerm;
		metalTerm += brdf * n_wi_Li;
		totalDiffuseTerm += diffuseTerm;
	}
	
	metalTerm *= u_Material.albedo.rgb;
	vec3 microfacetTerm = u_Material.metalness * metalTerm + (1.0 - u_Material.metalness) * dielectricTerm;
	out_Color = u_Material.reflectivity * microfacetTerm + (1.0 - u_Material.reflectivity) * totalDiffuseTerm;
}