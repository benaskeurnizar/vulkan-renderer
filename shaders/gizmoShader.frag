#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 p = normalize(fragPos);
    vec3 color = fragColor; // grey base

    float ringWidth = 0.02;
    if (abs(p.x) < ringWidth) color = vec3(0.0, 1.0, 0.0); // green ring: x≈0 plane (yz)
    else if (abs(p.y) < ringWidth) color = vec3(1.0, 0.0, 0.0); // red ring: y≈0 plane (xz)

    outColor = vec4(color, 1.0);
    
    //outColor = vec4(fragColor, 0.96);
}