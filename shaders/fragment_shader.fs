#version 330 core
out vec4 FragColor;

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

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 viewPos; 
uniform vec3 objectColor;

uniform SpotLight light;
uniform PointLight pointLight;

uniform int objectShine;

// mg³a
in float visibility;
vec3 skyColor = vec3(1.0, 1.0, 1.0);

// noc
uniform float nightFactor;
vec3 nightGlow = vec3(0.1, 0.6, 1.0);

void main()
{
       vec3 viewDir = normalize(viewPos - FragPos);

    
        vec3 color = CalcSpotLight(light, Normal, FragPos, viewDir);
        //color += CalcPointLight(pointLight, Normal, FragPos, viewDir);

        // noc
        color = mix(color, nightGlow, nightFactor / 4);
        color *= (1 - nightFactor / 2);

        // dodanie mg³y - wymieszanie koloru mg³y z kolorem obiektu
        color = mix(skyColor, color, visibility);

        FragColor = vec4(color, 1.0);
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
       vec3 reflectDir = reflect(-lightDir, normal);  
       float spec = pow(max(dot(viewDir, reflectDir), 0.0), objectShine);
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
   
       vec3 result = (ambient + diffuse + specular) * objectColor;

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
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), objectShine);
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
    return (ambient + diffuse + specular) * objectColor;
}