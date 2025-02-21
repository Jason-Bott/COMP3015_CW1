#version 460

in vec3 Position;
in vec3 Normal;
in vec3 SkyBoxVec;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform samplerCube SkyBoxTex;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
} lights[6];

uniform struct MaterialInfo {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
} Material;

vec3 blinnPhong(int light, vec3 position, vec3 n) {
    vec3 diffuse = vec3(0), spec = vec3(0);
    vec3 ambient = lights[light].La * Material.Ka;
    vec3 s = normalize(vec3(lights[light].Position.xyz) - position);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = Material.Kd * sDotN;
    if(sDotN > 0.0) {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }
    return ambient + (diffuse + spec) * lights[light].L;
}

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(SkyBoxVec)).rgb;

    vec3 Color = vec3(0.0);
    for (int i = 0; i < 6; i++) {
        Color += blinnPhong(i, Position, Normal);
    }

    float Gamma = 2.2f;

    bool isSky = length(Position) > 30.0;
    vec3 finalColor = isSky ? texColor : pow(Color, vec3(1.0/Gamma));

    FragColor = vec4(finalColor, 1.0);
}
