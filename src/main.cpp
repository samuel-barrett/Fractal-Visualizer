/**
 * Main application code:
 * Samuel Barrett, Seng 475, Summer 2021
*/

//GL includes
#include <GL/glew.h>
#include <GL/freeglut.h>


//Std includes
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <complex>
#include <regex>
#include <cmath>

#include "ra/henon.hpp"
#include "ra/shaders.hpp"

using std::cout, std::endl, std::size_t;
using point = ra::fractal_logic::point<long double>;
using fractal_t = ra::fractal_logic::henon_map<long double>::fractal_t;

/**
 * Descriptions: This class contains only static variables and functions. It is used to contain
 * some call back functions within a scope, so that global variables are not needed.
 * 
 * Contains functions four mouseCallback, displayCallback, 
 */
class call_back_funcs {

    public:

    enum mouse_state_t {
        right_button,
        left_button,
        scroll_up,
        scroll_down,
        none
    };

    static ra::fractal_logic::henon_map<long double> henon;

    static volatile int mouse_down_x, mouse_down_y;
    static volatile int pan_pos_x, pan_start_pos_y;

    static volatile mouse_state_t mouse_state;

    static unsigned int list_index;

    static int min_loc, max_loc;
    static int screen_pixels_loc;

    /**
     * Code to process display call
     * Sends henon min and max location to shaders
     * then draws the elements
     */
    static void display_func() {
        //Set world dimensions
        glUniform2f(min_loc, henon.get_bottom_left().x, henon.get_bottom_left().y);
        glUniform2f(max_loc, henon.get_top_right().x, henon.get_top_right().y);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //cout << "(" << henon.get_bottom_left() << "," << henon.get_top_right() << ")" << endl;

        glutSwapBuffers();
    }

    /**
     * Code to process change of window size
     * Scales viewing region by amount of increase in size.
     * Keeping center region the same
     */
    static void reshape_func(int new_x_pixels,int new_y_pixels) {

        //Set scale factors
        long double sf_x = static_cast<double>(new_x_pixels)/henon.get_x_pixels();
        long double sf_y = static_cast<double>(new_y_pixels)/henon.get_y_pixels();

        point min = henon.get_bottom_left();
        point max = henon.get_top_right();

        point center((max.x+min.x)/2.0, (max.y+min.y)/2.0);

        long double x_half_range = sf_x*(max.x-min.x)/2.0;
        long double y_half_range = sf_y*(max.y-min.y)/2.0;

        long double min_x = center.x - x_half_range;
        long double max_x = center.x + x_half_range;

        long double min_y = center.y - y_half_range;
        long double max_y = center.y + y_half_range;

        //Reset fractal scale with parameters scaled to new size of screen
        henon.set_bottom_left({min_x, min_y});
        henon.set_top_right({max_x, max_y});


        henon.set_x_pixels(new_x_pixels);
        henon.set_y_pixels(new_y_pixels);

        //Update shader with new info
        glUniform2ui(screen_pixels_loc, henon.get_x_pixels(), henon.get_y_pixels());

        //Set world dimensions
        glUniform2f(min_loc, henon.get_bottom_left().x, henon.get_bottom_left().y);
        glUniform2f(max_loc, henon.get_top_right().x, henon.get_top_right().y);

        //Allocate space for new pixels if needed
        //henon.set_vector_size();

        glViewport(0,0,henon.get_x_pixels(), henon.get_y_pixels());

        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //glutPostRedisplay();
        //glutSwapBuffers();
    }

    /**
     * Code to process translation of viewing region by mouse
     * xs,ys: starting coordinate 
     * xe,ye: ending cooridnates
     * min/max: Starting coordinate of view
     */
    static void mouse_translate(int xs, int ys, int xe, int ye, 
        point& min, point& max) {

        
        point ps = henon.map_to_cartesian_plane(xs, ys);
        point pe = henon.map_to_cartesian_plane(xe, ye);

        henon.set_x_params(min.x - pe.x + ps.x, max.x - pe.x + ps.x);
        henon.set_y_params(min.y - pe.y + ps.y, max.y - pe.y + ps.y);
    }

    /**
     * Code to process zooming viewing region by mouse
     * sf: Amount to zoom mouse by
     */
    static void mouse_zoom(double sf, point& min, point& max) {
        long double x_dist = max.x - min.x;
        long double y_dist = max.y - min.y;

        henon.set_bottom_left({min.x-x_dist*sf, min.y-y_dist*sf});
        henon.set_top_right({max.x+x_dist*sf, max.y+y_dist*sf});
    }

    /**
     * Code to process selection of viewing region by mouse
     * xs,ys: starting coordinate 
     * xe,ye: ending cooridnates
     */
    static void mouse_select(int xs, int ys, int xe, int ye) {

        double selection_aspect_ratio = std::abs(static_cast<double>(ys - ye)/(xs-xe));
        double screen_aspect_ratio = static_cast<double>(henon.get_y_pixels())/henon.get_x_pixels();

        int min_x, max_x, x_selection_size;
        int min_y, max_y, y_selection_size;

        double middle_x, middle_y;

        //Selection is larger relatively in x direction than in y
        if(selection_aspect_ratio < screen_aspect_ratio) {
            x_selection_size = std::abs(xe-xs);
            y_selection_size = x_selection_size*screen_aspect_ratio;

        } else {
            y_selection_size = std::abs(ye-ys);
            x_selection_size = y_selection_size/screen_aspect_ratio;
        }

        middle_y = (ys+ye)/2.0;
        min_y    =  middle_y - y_selection_size/2.0;
        max_y    =  middle_y + y_selection_size/2.0;

        middle_x = (xs+xe)/2.0;

        min_x    =  middle_x - x_selection_size/2.0;
        max_x    =  middle_x + x_selection_size/2.0;

        auto lower_point = henon.map_to_cartesian_plane(min_x, min_y);
        auto upper_point = henon.map_to_cartesian_plane(max_x, max_y);
        
        henon.set_bottom_left(lower_point);
        henon.set_top_right(upper_point);
    }


    /**
     * Called upon keyboard event, performs transformation of viewing region (zoom in/out or exit) depending on 
     * key press
     */
    static void keyboard_func (unsigned char key, int, int) {

        constexpr char ESCAPE = 27;

        point min(henon.get_bottom_left());
        point max(henon.get_top_right());

        switch (key) {
            case 'x': case 'X': //Zoom out
                mouse_zoom(0.1, min, max);
                break;
            case 'z': case 'Z': //Zoom in
                mouse_zoom(-0.1, min, max);
                break;
            case 'r': case 'R': { //Reset
                henon.set_bottom_left(henon.get_start_bottom_left());
                henon.set_top_right(henon.get_start_top_right());
                break;
            }
            case ESCAPE: {
                glutDestroyWindow(glutGetWindow());
                return;
            }
        }
        glutPostRedisplay();
    }

    /**
     * Called upon mouse event, performs transformation of viewing region depending on operation
     */
    static void mouse_func(int button, int state, int x, int y) {

        constexpr int MOUSE_SCROLL_UP = 3;
        constexpr int MOUSE_SCROLL_DOWN = 4;

        //Want lower value of y to correspond to lower on screen
        //And higher to higher on screen
        y = henon.get_y_pixels()-1-y;

        point min(henon.get_bottom_left());
        point max(henon.get_top_right());

        switch(button) {
            case GLUT_LEFT_BUTTON: { // Pan
                if(mouse_state != none && mouse_state != left_button) {
                    return; //Not the right state
                }
                
                if(state == GLUT_DOWN) {
                    mouse_state = left_button;
                    mouse_down_x = x;
                    mouse_down_y = y;
                } else if(state == GLUT_UP) {
                    mouse_translate(mouse_down_x, mouse_down_y, x, y, min, max);
                    mouse_state = none;
                }
            }
            break;
            case GLUT_RIGHT_BUTTON: {
                if(mouse_state != none && mouse_state != right_button) return; //Not the right state
                
                if(state == GLUT_DOWN) { // Zoom in 
                    mouse_state = right_button;
                    mouse_down_x = x;
                    mouse_down_y = y;
                } else if(state == GLUT_UP){
                    mouse_select(mouse_down_x, mouse_down_y, x, y);
                    mouse_state = none;
                }
            }
            break;
            case MOUSE_SCROLL_DOWN: {
                if(mouse_state != none && mouse_state != scroll_down) return; //Not the right state
                
                mouse_state = scroll_down;

                //Return for rendent mouse scroll
                if(state == GLUT_DOWN) return;

                mouse_zoom(0.1, min, max);

                mouse_state = none;
            }
            break;
            case MOUSE_SCROLL_UP: {
                if(mouse_state != none && mouse_state != scroll_up) return; //Not the right state

                mouse_state = scroll_up;

                if(state == GLUT_DOWN) return;

                mouse_zoom(-0.1, min, max);

                mouse_state = none;
            }
        }
        glutPostRedisplay();
    }
};

ra::fractal_logic::henon_map<long double> call_back_funcs::henon;

volatile int call_back_funcs::mouse_down_x, call_back_funcs::mouse_down_y;
volatile int call_back_funcs::pan_pos_x, call_back_funcs::pan_start_pos_y;

volatile call_back_funcs::mouse_state_t call_back_funcs::mouse_state = call_back_funcs::mouse_state_t::none;

unsigned int call_back_funcs::list_index;

int call_back_funcs::min_loc, call_back_funcs::max_loc;
int call_back_funcs::screen_pixels_loc;


/*************************************************
**************FUNCTIONS***************************
**************************************************/

/** Print usage from processing command line arguments to terminal
 *  Param [in] (std::string) name: The name of the program (argv[0])
 *  Return error code.
 */
static int show_usage(std::string name) {
    cout<< "Usage: " << name << " <options>" << endl
        << "Options:\n"
        << "\t-h [height]\tSpecify window height in pixels\n"
        << "\t-w [width]\tSpecify window width  in pixels\n"
        << "\t-a [alpha]\tSpecify henon alpha (or a) value\n"
        << "\t-b [beta]\tSpecify henon beta (or b) value\n"
        << "\t-t [threshold]\tSpecify value at which point the point is assumed to have escaped\n"
        << "\n\t-L [leftmost point],[lowest point]:\n\t\tSpecify bottom left point to display initially on xy plane\n"
        << "\t-U [rightmost point],[highest point]:\n\t\tSpecify top right point to display initially on xy plane\n"
        << "\t-f [fractal]:\tSpecify top right point to display initially on xy plane\n"
        << "\t\tArguments:\n\t\tmandelbrot: display mandelbrot set\n\t\thenon: display henon (default)\n";

    return -1;
}


/**
 * Compile and create shader object
 * 
 * const char * shader: Pointer to shader string code
 * GLenum type: Shader type (i.e. fragment, vertex, etc)
 * 
 * Return: id of shader
 */
unsigned int compile_shader(const char * shader, GLenum type) {
    int compile_status;

    unsigned int shader_id = glCreateShader(type);

    if(!shader_id) {
        std::cerr << "Error compiling shader" << endl;
        return 0;
    }

    glShaderSource(shader_id, 1, &shader, NULL);
    glCompileShader(shader_id);


    // check for compilation status
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);

    if(!compile_status) {
        int length;
        char *msg = new char[length];

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        glGetShaderInfoLog(shader_id, length, &length, msg);

        cout << "Shader did not compile. MSG: "<< endl<< shader<<endl << msg;
        
        delete[] msg;
        
        glDeleteShader(shader_id);
        
        return 0;
    }

    return shader_id;
}

/**
 * Attachers shaders into program creates program, and returns id
 */
unsigned int link_program(unsigned int vert_shader_id, unsigned int frag_shader_id) {
    int link_status = 0;
    
    unsigned int program_id = glCreateProgram();

    if (!program_id) {
        std::cerr << "Error Creating Shader Program" << endl;
        return 0;
    }

    glAttachShader(program_id, vert_shader_id);
    glAttachShader(program_id, frag_shader_id);

    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        std::cerr << "Error Linking program" << endl;
        glDetachShader(program_id, vert_shader_id);
        glDetachShader(program_id, frag_shader_id);
        glDeleteProgram(program_id);

        return 0;
    }

    return program_id;
}

/**
 * Gets locations of uniforms from shader and set their initial values
 * 
 * input: (int program id): The id of the program
 * 
 * return 0 for success, -1 for failure
 */
int set_uniforms(int program_id) {
    int uniform_loc;//Id for uniforms that only need to be accessed at init
    
    //Set max iters
    uniform_loc = glGetUniformLocation(program_id, "max_its");
    if(uniform_loc == -1) {
        std::cerr << "Could not get max_its location" <<endl;
        return -1;
    }
    glUniform1i(uniform_loc, call_back_funcs::henon.get_max_iterations());

    call_back_funcs::min_loc = glGetUniformLocation(program_id, "min");
    if(call_back_funcs::min_loc == -1) {
        std::cerr << "Could not get min location" <<endl;
        return -1;
    }
    glUniform2d(call_back_funcs::min_loc, call_back_funcs::henon.get_bottom_left().x, call_back_funcs::henon.get_bottom_left().y);


    call_back_funcs::max_loc = glGetUniformLocation(program_id, "max");
    if(call_back_funcs::max_loc == -1) {
        std::cerr << "Could not get max location" <<endl;
        return -1;
    }
    glUniform2d(call_back_funcs::max_loc, call_back_funcs::henon.get_top_right().x, call_back_funcs::henon.get_top_right().y);


    call_back_funcs::screen_pixels_loc = glGetUniformLocation(program_id, "screen_pixels");
    if(call_back_funcs::screen_pixels_loc == -1) {
        std::cerr << "Could not get screen_pixels location" <<endl;
        return -1;
    }
    glUniform2ui(call_back_funcs::screen_pixels_loc, call_back_funcs::henon.get_x_pixels(), call_back_funcs::henon.get_y_pixels());

    //Variables only need for henon fractal
    if(call_back_funcs::henon.get_fractal_type() == fractal_t::henon) {
        
        uniform_loc = glGetUniformLocation(program_id, "a");
        
        if(uniform_loc == -1) {
            std::cerr << "Could not get \"a\" location" <<endl;
            return -1;
        }
        glUniform1d(uniform_loc, call_back_funcs::henon.get_a());

        
        uniform_loc = glGetUniformLocation(program_id, "b");
        
        if(uniform_loc == -1) {
            std::cerr << "Could not get b location" << endl;
            return -1;
        }
        glUniform1d(uniform_loc, call_back_funcs::henon.get_b());

        
        uniform_loc = glGetUniformLocation(program_id, "threshold");
        
        if(uniform_loc == -1) {
            std::cerr << "Could not get threshold location" << endl;
            return -1;
        }
        glUniform1d(uniform_loc, call_back_funcs::henon.get_threshold());
    }

    return 0;
}

/** Load and initialize shaders
 * Screen is filled with two triangles with shared hypoteneuse.
 * fragment shaders used to color based on fractal
*/
int init_shaders() {  
    unsigned int vbo_id, vao_id, ebo_id; //Vertex ids
    unsigned int program_id;
    unsigned int vertex_shader_id, frag_shader_id; //Shader ids

    //Num floats for two triangle vertices
    const std::size_t num_vertices_floats = 12;
    const std::size_t num_indices = 6;

    // triangle vertex coordinates
    const float vertices[num_vertices_floats] = {
        +1.0f, +1.0f, +0.0f,
        +1.0f, -1.0f, +0.0f,
        -1.0f, -1.0f, +0.0f,
        -1.0f, +1.0f, +0.0f
    };

    //Specify indices for triangles to be drawn
    const unsigned int indices[num_indices] {
        0,1,3, //First triangle
        1,2,3 //Second
    };

    //in vec locations
    unsigned int pos_attr_loc;

    //Link shaders
    if(call_back_funcs::henon.get_fractal_type() == fractal_t::henon) {
        const char * vert_shader = HENON_VERTEX_SHADER.c_str();
        const char * frag_shader = HENON_FRAGMENT_SHADER.c_str();


        vertex_shader_id = compile_shader(vert_shader, GL_VERTEX_SHADER);
        if(!vertex_shader_id) {
            std::cerr << "Could not compile vertex shader" << endl;
            return -1;
        }

        frag_shader_id =   compile_shader(frag_shader, GL_FRAGMENT_SHADER);
        if(!frag_shader_id) {
            std::cerr << "Could not compile fragment shader" << endl;
            return -1;
        }
    } else if(call_back_funcs::henon.get_fractal_type() == fractal_t::mandelbrot) {
        const char * vert_shader = MANDELBROT_VERTEX_SHADER.c_str();
        const char * frag_shader = MANDELBROT_FRAGMENT_SHADER.c_str();


        vertex_shader_id = compile_shader(vert_shader, GL_VERTEX_SHADER);
        if(!vertex_shader_id) {
            std::cerr << "Could not compile vertex shader" << endl;
            return -1;
        }

        frag_shader_id =   compile_shader(frag_shader, GL_FRAGMENT_SHADER);
        if(!frag_shader_id) {
            std::cerr << "Could not compile fragment shader" << endl;
            return -1;
        }
    } else{
        return -1;
    }

    program_id = link_program(vertex_shader_id, frag_shader_id);
    if(!frag_shader_id) {
        std::cerr << "Could link compile fragment shader" << endl;
        return -1;
    }


    //Create and bind vertex array
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);
    glGenBuffers(1, &ebo_id);

    //Bind vertex array object first
    glBindVertexArray(vao_id);

    //Copy vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices_floats, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices*sizeof(unsigned int), indices, GL_STATIC_DRAW);


    // Setup pos_attr buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    pos_attr_loc = glGetAttribLocation(program_id, "pos_attr");
    glVertexAttribPointer(pos_attr_loc, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(pos_attr_loc);

    // Use this program for rendering.
    glUseProgram(program_id);

    //Uniform locations are aquired from shader here
    return set_uniforms(program_id);
}

int main(int argc, char ** argv) {

    if(call_back_funcs::henon.process_command_line_args(argc, argv) < 0) {
        return show_usage(argv[0]);
    }

    glutInit(&argc,argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitContextVersion(4,1);

    glutInitWindowSize(call_back_funcs::henon.get_x_pixels(), call_back_funcs::henon.get_y_pixels());

    if(call_back_funcs::henon.get_fractal_type() == fractal_t::henon) {
        glutCreateWindow("Henon Fractal");
    } else if (call_back_funcs::henon.get_fractal_type() == fractal_t::mandelbrot) {
        glutCreateWindow("Mandelbrot Fractal");
    }
    glewInit();

    if(init_shaders() < 0) {
        return -1;
    }

    //Call call-back functions
    glutDisplayFunc(&call_back_funcs::display_func);
    glutKeyboardFunc(&call_back_funcs::keyboard_func);
    glutMouseFunc(&call_back_funcs::mouse_func);
    glutReshapeFunc(&call_back_funcs::reshape_func);

    glutMainLoop();


    return 0;
}
