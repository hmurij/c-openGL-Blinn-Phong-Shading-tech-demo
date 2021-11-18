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

// Light and material properties
uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
uniform vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 128.0;
uniform vec3 ambient = vec3(0.1, 0.1, 0.1);

uniform vec3 viewPos;


// Outputs to the fragment shader
out VS_OUT
{
    flat vec3 color_flat;
    vec3 color;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 P = view * model * position;

    // Calculate normal in view space
    vec3 N = mat3(model) * normal;
    // Calculate view-space light vector
    vec3 L = light_pos - P.xyz;
    // Calculate view vector (simply the negative of the view-space position)
    // vec3 V = -P.xyz;
    vec3 V = viewPos - ((model) * position).xyz;

    // Normalize all three vectors
    N = normalize(N);
    L = normalize(L);
    V = normalize(V);
    // Calculate R by reflecting -L around the plane defined by N
    vec3 R = reflect(-L, N);
    

    // Calculate the diffuse and specular contributions
    vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
    vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;
    
    // Send the color output to the fragment shader
    vs_out.color = vs_out.color_flat = ambient + diffuse + specular;

    // Calculate the clip-space position of each vertex
    gl_Position = projection * P;
    // gl_Position = projection * view * P;

}

#fragment shader
#version 410 core

// Output
layout(location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    flat vec3 color_flat;
    vec3 color;
} fs_in;

uniform bool flat_color;

void main(void)
{
    // Write incoming color to the framebuffer
    if(flat_color)
        color = vec4(fs_in.color_flat, 1.0);
    else
        color = vec4(fs_in.color, 1.0);
}
