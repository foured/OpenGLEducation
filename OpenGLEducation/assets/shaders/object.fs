#version 330 core 
out vec4 FragColor;

struct Material{
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform sampler2D diffuse0;
uniform sampler2D specular0;

#define MAX_POINT_LIGHTS 20 
struct PointLight{
	vec3 position;

	float k0;
	float k1;
	float k2;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

struct DirLight{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	sampler2D depthBuffer;
	mat4 lightSpaceMatrix;
};

uniform DirLight dirLight;

#define MAX_SPOT_LIGHTS 5
struct SpotLight{
	vec3 direction;
	vec3 position;
	
	float k0;
	float k1;
	float k2;

	float cutOff;
	float outerCutOff;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float nearPlane;
	float farPlane;

	sampler2D depthBuffer;
	mat4 lightSpaceMatrix;
};

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform int noTex;
uniform vec3 viewPos;

uniform bool useBlinn;
uniform bool useGamma;

float calcSpotLightShadow();
float calcDirLightShadow();
vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);

void main(){
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec4 diffMap;
	vec4 specMap;

	if(noTex == 1){
		diffMap = material.diffuse;
		specMap = material.specular;
	}
	else{
		diffMap = texture(diffuse0, TexCoord);
		specMap = texture(specular0, TexCoord);
	}

	//plaseholder
	vec4 result;

	//dirLight
	result = calcDirLight(norm, viewDir, diffMap, specMap);

	//pointLights
	for(int i = 0; i < noPointLights; i ++){
		result += calcPointLight(i, norm, viewDir, diffMap, specMap);
	}

	//spotLights
	for(int i = 0; i < noSpotLights; i ++){
		result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
	}

	if(useGamma){
		float gamma = 2.2;
		result.rgb = pow(result.rgb, vec3(1.0 / gamma));
	}

	// depth testing
	float near = 0.1;
	float far = 100.0;
	float z = gl_FragCoord.z * 2.0 - 1.0; // transform to NDC [0, 1] => [-1, 1]
	float linearDepth = (2.0 * near * far) / (z * (far - near) - (far + near)); // take inverse of the projection matrix (perspective)
	float factor = (near + linearDepth) / (near - far); // convert back to [0, 1]

	result.rgb *= 1 - factor;

	FragColor = result;
}

float calcDirLightShadow(vec3 norm, vec3 lightDir){
	vec4 fragPosLightSpace = dirLight.lightSpaceMatrix * vec4(FragPos, 1.0);

	//perspective devide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // [depth relative to light] => [-1, 1]

	//NDC to depht range
	projCoords = projCoords * 0.5 + 0.5; //[-1, 1] => [0, 1]

	//if to far return no shadow
	if(projCoords.z > 1.0){
		return 0.0;
	}

	//get the closest 
	float closestDepth = texture(dirLight.depthBuffer, projCoords.xy).r;

	//get depth of fragment
	float currentDepth = projCoords.z;

	//calulate bias
	float maxBais = 0.05;
	float minBais = 0.005;
	float bais = max(maxBais * (1.0 - dot(norm, lightDir)), minBais);

	//PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirLight.depthBuffer, 0);
	for(int y = -1; y <= 1; y++ ){
		for(int x = -1; x <= 1; x++ ){
			float pcfDepth = texture(dirLight.depthBuffer, projCoords.xy + vec2(x,y) * texelSize).r;
			shadowSum += currentDepth - bais > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadowSum / 9.0;
	//return currentDepth - bais > closestDepth ? 1.0 : 0.0;
}

float calcSpotLightShadow(int idx, vec3 norm, vec3 lightDir){
	vec4 fragPosLightSpace = spotLights[idx].lightSpaceMatrix * vec4(FragPos, 1.0);

	//perspective devide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // [depth relative to light] => [-1, 1]

	//NDC to depht range
	projCoords = projCoords * 0.5 + 0.5; //[-1, 1] => [0, 1]

	//if to far return no shadow
	if(projCoords.z > 1.0){
		return 0.0;
	}

	//get the closest 
	float closestDepth = texture(spotLights[idx].depthBuffer, projCoords.xy).r;

	//linearize depth
	float z = closestDepth * 2.0 - 1.0;
	closestDepth = (2.0 * spotLights[idx].nearPlane * spotLights[idx].farPlane) /
		(spotLights[idx].farPlane + spotLights[idx].nearPlane - z * (spotLights[idx].farPlane - spotLights[idx].nearPlane));
	closestDepth /= spotLights[idx].farPlane;

	//get depth of fragment
	float currentDepth = projCoords.z;

	//calulate bias
	float maxBais = 0.05;
	float minBais = 0.005;
	float bais = max(maxBais * (1.0 - dot(norm, lightDir)), minBais);

	//PCF
	float shadowSum = 0.0;
	vec2 texelSize = 1.0 / textureSize(spotLights[idx].depthBuffer, 0);
	for(int y = -1; y <= 1; y++ ){
		for(int x = -1; x <= 1; x++ ){
			float pcfDepth = texture(spotLights[idx].depthBuffer, projCoords.xy + vec2(x,y) * texelSize).r;
			shadowSum += currentDepth - bais > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadowSum / 9.0;
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap){
	//ambient
	vec4 ambient = pointLights[idx].ambient * diffMap;

	//diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos);
	float dif = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = pointLights[idx].diffuse * (dif * diffMap);

	//specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if(dif > 0){
		float dotProd = 0;
		if(useBlinn){
			//calculate using Blinn-Phing model
			vec3 halfwayDir = normalize(lightDir + viewDir);
			dotProd = dot(norm, halfwayDir);
		}
		else{
			//calculate using Phong model	
			vec3 reflectDir = reflect(-lightDir, norm);
			dotProd = dot(viewDir, reflectDir);
		}

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * (dist * dist));

	return vec4(ambient + diffuse + specular) * attenuation;
}

vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap){
	//ambient
	vec4 ambient = dirLight.ambient * diffMap;

	//diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float dif = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = dirLight.diffuse * (dif * diffMap);

	//specular
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	if(dif > 0){
		float dotProd = 0;
		if(useBlinn){
			//calculate using Blinn-Phing model
			vec3 halfwayDir = normalize(lightDir + viewDir);
			dotProd = dot(norm, halfwayDir);
		}
		else{
			//calculate using Phong model	
			vec3 reflectDir = reflect(-lightDir, norm);
			dotProd = dot(viewDir, reflectDir);
		}

		float spec = pow(max(dotProd, 0.0), material.shininess * 128);
		specular = dirLight.specular * (spec * specMap);
	}

	float shadow = calcDirLightShadow(norm, lightDir);

	return vec4(ambient + (1.0 - shadow) * (diffuse + specular));
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap) {
	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	// ambient
	vec4 ambient = spotLights[idx].ambient * diffMap;

	if (theta > spotLights[idx].outerCutOff) {
		// if in cutoff, render light
		// > because using cosines and not degrees

		// diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		vec4 diffuse = spotLights[idx].diffuse * (diff * diffMap);

		// specular
		vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
		if(diff > 0){
			float dotProd = 0;
			if(useBlinn){
				//calculate using Blinn-Phing model
				vec3 halfwayDir = normalize(lightDir + viewDir);
				dotProd = dot(norm, halfwayDir);
			}
			else{
				//calculate using Phong model	
				vec3 reflectDir = reflect(-lightDir, norm);
				dotProd = dot(viewDir, reflectDir);
			}

			float spec = pow(max(dotProd, 0.0), material.shininess * 128);
			specular = dirLight.specular * (spec * specMap);
		}

		// calculate Intensity
		float intensity = clamp((theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff), 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * (dist * dist));

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		float shadow = calcSpotLightShadow(idx, norm, lightDir);

		return vec4(ambient + (1.0 - shadow) * (diffuse + specular));
	}
	else {
		// render just ambient
		return ambient;
	}
}