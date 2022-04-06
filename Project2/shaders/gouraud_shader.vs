#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

struct PointLight {
    vec3 position;  
  
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;  
  
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
	
    float constant;
    float linear;
    float quadratic;

    vec3 direction;
    float cutOff;
    float outerCutOff;
};

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec2 TexCoords;
out vec3 LightingColor; // resulting color from lighting calculations


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// parametry dla mg³y
float visibility;
const float density = 0.015;
const float gradient = 1.5;

vec3 Normal;  
vec3 FragPos;  

uniform vec3 viewPos; 

uniform SpotLight light;
uniform PointLight pointLight;
uniform PointLight secondPointLight;

uniform int objectShine;

// mg³a
vec3 skyColor = vec3(1.0, 1.0, 1.0);

// noc
uniform float nightFactor;
vec3 nightGlow = vec3(0.1, 0.6, 1.0);

uniform bool blinn;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    vec4 positionRelativeToCam = view * vec4(model * vec4(aPos, 1.0));
    float distance = length(positionRelativeToCam.xyz);
    visibility = exp(-pow((distance*density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    Normal = mat3(transpose(inverse(model))) * aNormal; 


    // fragment shader

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 color = CalcSpotLight(light, Normal, FragPos, viewDir);
    color += CalcPointLight(pointLight, Normal, FragPos, viewDir);
    color += CalcPointLight(secondPointLight, Normal, FragPos, viewDir);

    // dodanie mg³y - wymieszanie koloru mg³y z kolorem obiektu
    color = mix(skyColor, color, visibility);

    // noc
    color = mix(color, nightGlow, nightFactor / 4);
    color *= (1 - nightFactor / 2);

    TexCoords = aTexCoords;
    LightingColor = color;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

vec3 CalcSpotLight(SpotLight light, vec3 Normal, vec3 FragPos, vec3 viewDir)
{
       vec3 ambient = light.ambient * light.color;
   
       // diffuse 
       vec3 normal = normalize(Normal);
       vec3 lightDir = normalize(light.position - FragPos);

       float diff = max(dot(normal, lightDir), 0.0);
       vec3 diffuse = light.diffuse * diff * light.color;
   
       // specular
       float spec;
       if(blinn)
       {
           vec3 halfwayDir = normalize(lightDir + viewDir); 
           spec = pow(max(dot(viewDir, halfwayDir), 0.0), objectShine);
       }
       else
       {
           vec3 reflectDir = reflect(-lightDir, normal);  
           spec = pow(max(dot(viewDir, reflectDir), 0.0), objectShine);
       }
       vec3 specular = light.specular * spec * light.color;  
       
       // spotlight (soft edges)
       float theta = dot(lightDir, normalize(-light.direction)); 
       float epsilon = (light.cutOff - light.outerCutOff);
       float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
       diffuse  *= intensity;
       specular *= intensity;

       // attenuation
       float distance    = length(light.position - FragPos);
       float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

       ambient  *= attenuation;  
       diffuse  *= attenuation;
       specular *= attenuation;
   
       vec3 result = ambient + diffuse + specular;

       return result;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 Normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 normal = normalize(Normal);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec;
    if(blinn)
    {
       vec3 halfwayDir = normalize(lightDir + viewDir); 
       spec = pow(max(dot(viewDir, halfwayDir), 0.0), objectShine);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);  
        spec = pow(max(dot(viewDir, reflectDir), 0.0), objectShine);
    }
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * light.color;
    vec3 diffuse = light.diffuse * diff * light.color;
    vec3 specular = light.specular * spec * light.color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}