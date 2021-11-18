#vertex shader
#version 410 core

// Per-vertex inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

// Matrices we'll need
layout(std140) uniform constants
{
    mat4 model;
    mat4 view;
    mat4 projection;
};

// Inputs from vertex shader
out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} vs_out;

// Position of light
uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
uniform vec3 viewPos;

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = view * model * position;

    // Calculate normal in view-space
    vs_out.N = mat3(model) * normal;

    // Calculate light vector
    vs_out.L = light_pos - P.xyz;

    // Calculate view vector
    // vs_out.V = -P.xyz;
    vs_out.V = viewPos - (model * position).xyz;

    // Calculate the clip-space position of each vertex
    gl_Position = projection * P;
}

#fragment shader
#version 410 core

// Output
layout(location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

// Material properties
uniform vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 200.0;
uniform vec3 ambient = vec3(0.1, 0.1, 0.1);

uniform bool ambient_enable = true;
uniform bool diffuse_enable = true;
uniform bool specular_enable = true;
uniform bool rim_enable = true;

uniform vec3 rim_color = vec3(0.1, 0.7, 0.2);
uniform float rim_power = 5.0;

vec3 calculate_rim(vec3 N, vec3 V)
{
    float f = 1.0 - dot(N, V);

    f = smoothstep(0.0, 1.0, f);
    f = pow(f, rim_power);

    return f * rim_color;
}

void main(void)
{
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    vec3 H = normalize(L + V);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
    vec3 specular = pow(max(dot(N, H), 0.0), specular_power) * specular_albedo;

    vec3 rim = calculate_rim(N, V);

    // Write final color to the framebuffer
    // color = vec4(ambient + diffuse + specular + rim, 1.0);
    color = vec4(ambient * float(ambient_enable) + diffuse * float(diffuse_enable) + specular * float(specular_enable) + rim * float(rim_enable), 1.0);
}
