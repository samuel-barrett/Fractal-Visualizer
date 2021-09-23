#ifndef RA_SHADERS_HPP
#define RA_SHADERS_HPP

/**
 * Description: header file to put definitions of various opengl shaders in.
 * These are kept here as opposed to files, so that no file io is needed.
 */

#include <string.h>

inline const std::string MANDELBROT_FRAGMENT_SHADER (R"(
#version 430

in vec3 vTexCoord;
out vec4 frag_color;

//Uniforms
uniform int max_its; //Max iteration count
uniform uvec2 screen_pixels; //Number of pixels of window size
uniform vec2 min;
uniform vec2 max;


dvec3 set_rgb(double normalized_scalar) {
    dvec3 rgb;

    rgb.x = 1.0-normalized_scalar/2.0;
    rgb.y = sin(float(normalized_scalar)*3.14);
    rgb.z = normalized_scalar/2.0;

    return rgb;
}

int compute_mandelbrot(dvec2 c) {
    dvec2 z = vec2(0.0,0.0);

    int i;
    for(i=0; i<max_its; ++i) {
        z = vec2(z.x*z.x - z.y*z.y + c.x, 2.0*z.x*z.y + c.y);

        if(z.x*z.x + z.y*z.y > 4.0) {
            break;
        }
    }
    return i;
}

void main() {
    dvec3 colors;
    dvec2 coords;
    dvec2 c;
    double i;
    //vec2 min;
    //vec2 max;

    coords.x = double(gl_FragCoord.x)/double(screen_pixels.x-1);
    coords.y = double(gl_FragCoord.y)/double(screen_pixels.y-1);
    
    c.x = min.x + coords.x*(max.x-min.x);
    c.y = min.y + coords.y*(max.y-min.y);

        
    i = double(compute_mandelbrot(c))/double(max_its);

    colors = set_rgb(i);

    frag_color = vec4(colors,1.0);
}
)");


inline const std::string MANDELBROT_VERTEX_SHADER (R"(
#version 430

in vec3 pos_attr;
in vec3 aTexCoord;

//Output to next stage in pipeline
out vec3 vTexCoord;

void main() {
    vTexCoord = aTexCoord;
    gl_Position = vec4(pos_attr, 1.0);
}
)");


inline const std::string HENON_FRAGMENT_SHADER (R"(
#version 430

in vec3 vTexCoord;
out vec4 frag_color;

//Uniforms
uniform int max_its; //Max iteration count
uniform uvec2 screen_pixels; //Number of pixels of window size
uniform vec2 min;
uniform vec2 max;
uniform double threshold;
uniform double a;
uniform double b;


dvec3 set_rgb(double normalized_scalar) {
    dvec3 rgb;

    rgb.x = 1.0-normalized_scalar/2.0;
    rgb.y = sin(float(normalized_scalar)*3.14);
    rgb.z = normalized_scalar/2.0;

    return rgb;
}

int compute_henon(dvec2 p) {
    double temp = 0.0;

    double threshold_squared = threshold*threshold;

    int i;
    for(i=0; i<max_its; ++i) {
        p =  vec2(1.0 - a*p.x*p.x + p.y, b*p.x);

        if(p.x*p.x + p.y*p.y > threshold_squared) {
            break;
        }
    }
    return i;
}

void main() {
    dvec3 colors;
    dvec2 coords;
    dvec2 c;
    double i;

    coords.x = double(gl_FragCoord.x)/double(screen_pixels.x-1);
    coords.y = double(gl_FragCoord.y)/double(screen_pixels.y-1);
    
    c.x = min.x + coords.x*(max.x-min.x);
    c.y = min.y + coords.y*(max.y-min.y);

        
    i = double(compute_henon(c))/double(max_its);

    colors = set_rgb(i);

    frag_color = vec4(colors,1.0);
}
)");

inline const std::string HENON_VERTEX_SHADER (R"(
#version 430

in vec3 pos_attr;
in vec3 aTexCoord;

//Output to next stage in pipeline
out vec3 vTexCoord;

void main() {
    vTexCoord = aTexCoord;
    gl_Position = vec4(pos_attr, 1.0);
}
)");

#endif