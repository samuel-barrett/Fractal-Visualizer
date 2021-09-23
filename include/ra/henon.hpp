/**
 * Classes to store and process fractal logic:
 * Samuel Barrett, Seng 475, Summer 2021
*/

#ifndef RA_FRACTAL_LOGIC_HENON_MAP_HPP
#define RA_FRACTAL_LOGIC_HENON_MAP_HPP

#include <vector>

namespace ra::fractal_logic {
    
    using std::cout, std::endl;

    /**
     * Simple class used to represent a point.
     * Contains two members which can 
     */
    template<class FLOAT_T>
    class point {
        public:
        FLOAT_T x;FLOAT_T y;
        point(FLOAT_T x_in=0.0, FLOAT_T y_in=0.0): x(x_in), y(y_in) {}
    };

    //Print point
    template<class FLOAT_T>
    std::ostream& operator<<(std::ostream& os, const point<FLOAT_T> & p) {
        os << '(' << p.x << ',' << p.y << ')';
        return os;
    }

    /**
     * Class: henon_map
     * 
     * Description: 
     * 
     */
    template<class FLOAT_T>
    class henon_map
    {

        public: 

        using size_type = std::size_t;

        enum fractal_t {
            henon,
            mandelbrot
        };

        private:
        
        //Henon parameters
        FLOAT_T a_,b_;

        //Lower left point and upper right point for viewing grid in fractal
        point<FLOAT_T> min_, max_;
        
        //Starting values
        point<FLOAT_T> start_min_, start_max_;

        //Threshold at which point henon blows up
        FLOAT_T threshold_;

        //Number of iterations to use
        int max_its_;

        //Number of x pixels and y pixels
        int x_pixels_, y_pixels_;

        //The type of fractal
        fractal_t fractal;

        public:

        //Constructor initializes a bunch of values with defaults
        //unless other provided
        henon_map(FLOAT_T alpha= 0.2, FLOAT_T beta=0.9991, 
            FLOAT_T x_min=-5.0, FLOAT_T x_max=5.0, 
            FLOAT_T y_min  = -5.0, FLOAT_T y_max=5.0,
            int threshold=512, int max_its=512, 
            int x_pixels = 512, int y_pixels = 512):
            
            a_(alpha), b_(beta),
            min_({x_min, y_min}),
            max_({x_max, y_max}),
            start_min_(min_), start_max_(max_),
            threshold_(threshold), max_its_(max_its),
            x_pixels_(x_pixels), y_pixels_(y_pixels),
            fractal(henon) {}

        /**
         * Function: sets fractal type to mandelbrot or henon
         */
        bool set_fractal_type(std::string type) {
            if(type == "henon") {
                fractal = henon;
                return true;
            }else if(type == "mandelbrot") {
                fractal = mandelbrot;
                return true;
            }

            return false;
        }

        /**
         * Process coordinate from command line argument (as string) with format
         * [x value],[y_value] and return point
         * 
         * Returns: the point convertex from a string to a numeric type
         */
        point<long double> cla_set_point(std::string coordinate) {
            const std::regex cord_regex("(-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)");
            
            std::smatch cord_match;

            if(std::regex_match(coordinate, cord_match, cord_regex)) {
                return {std::stold(cord_match[1].str()), std::stold(cord_match[2].str())};
            }

            throw std::invalid_argument("Could not process coordinate setting");
        } 


        int process_command_line_args(int argc, char ** argv) {
            char * end;
            for(int i=1; i<argc; i+=2){ 
                std::string arg = argv[i];

                //Check for any format errors
                if(arg.size() != 2 || arg[0] != '-' || i >= argc-1) return -1;

                switch(arg[1]) {
                    case 'h': //Set screen height in pixels
                        try{
                            y_pixels_ = std::strtoull(argv[i+1], &end, 10);
                        }catch (std::invalid_argument& e) {
                            cout<<e.what()<<endl;
                            return -1;
                        }
                        break;
                    case 'w':
                        try{ //Set screen width in pixels
                            x_pixels_ = std::strtoull(argv[i+1], &end, 10);
                        } catch (std::invalid_argument& e){
                            cout<<e.what()<<endl; 
                            return -1;
                        }
                        break;
                    case 'a': //Set a parameter for henon map
                        try{
                            a_ = std::stold(argv[i+1]);
                        } catch (std::invalid_argument& e){
                            cout<<e.what()<<endl; 
                            return -1;
                        }
                        break;
                    case 'b':
                        try{
                            b_ = std::stold(argv[i+1]);
                        } catch (std::invalid_argument& e) {
                            cout<<e.what()<<endl; 
                            return -1;
                        }
                        break;
                    case 't': //Set threshold
                        try{
                            threshold_= std::stold(argv[i+1]);
                        } catch (std::invalid_argument& e){
                            cout<<e.what()<<endl; 
                            return -1;
                        }
                        break;
                    case 'm': //Set max iterations
                        try{
                            max_its_ = std::strtoull(argv[i+1], &end, 10); 
                        } catch (std::invalid_argument& e) {
                            cout<<e.what()<<endl; 
                            return -1;
                        }
                        break;
                    case 'f': { //Set fractal type
                        auto valid = set_fractal_type(argv[i+1]);
                        if(!valid) {
                            return -1;
                        }
                        break;
                    }
                    case 'L': {
                        min_ = cla_set_point(argv[i+1]);
                        break;
                    }
                    case 'U': {
                        max_ = cla_set_point(argv[i+1]);
                        break;  
                    }
                    default:
                        return -1;
                }
                    
            }

            if(get_fractal_type() == henon) {
                cout << "Displaying henon fractal:" << endl;
                cout << "a: " << get_a() << endl;
                cout << "b: " << get_b() << endl;
                cout << "Threshold: " << get_threshold() << endl;
            } else if(get_fractal_type() == mandelbrot) {
                cout << "Displaying mandelbrot fractal:" << endl;
            }
            cout << "Number of vertical pixels on screen: " << get_y_pixels() << endl;
            cout << "Number of vertical pixels on screen: " << get_x_pixels() << endl;
            cout << "Max Iterations: " << get_max_iterations() << endl;
            cout << "Lower Left Point: " << get_bottom_left() << endl;
            cout << "Upper Right Point: " << get_top_right() << endl;

            //Set start coordinates
            start_min_ = min_;
            start_max_ = max_;

            return 0;
        }


        fractal_t get_fractal_type() {
            return fractal;
        }

        void set_x_pixels(int x_pixels){x_pixels_ = x_pixels;}
        int get_x_pixels() const {return x_pixels_;}
        
        void set_y_pixels(int y_pixels){y_pixels_ = y_pixels;}
        int get_y_pixels()const {return y_pixels_;}

        void set_a(FLOAT_T a){a_ = a;}
        FLOAT_T get_a() const {return a_;}

        void set_b(FLOAT_T b){b_ = b;}
        FLOAT_T get_b() const {return b_;}

        void set_threshold(FLOAT_T threshold){threshold_ = threshold;}
        FLOAT_T get_threshold() const {return threshold_;}

        void set_max_iterations(int max_its){max_its_ = max_its;}
        int get_max_iterations() const {return max_its_;}

        void set_bottom_left(point<FLOAT_T> bottom_left) {min_ = bottom_left;}
        point<FLOAT_T> get_bottom_left() const {return min_;}

        point<FLOAT_T> get_start_bottom_left() const {return start_min_;}

        void set_top_right(point<FLOAT_T> top_right) {max_ = top_right;}
        point<FLOAT_T> get_top_right() const {return max_;}

        point<FLOAT_T> get_start_top_right() const {return start_max_;}

        void set_x_params(FLOAT_T min, FLOAT_T max) {min_.x = min; max_.x = max;}
        void set_y_params(FLOAT_T min, FLOAT_T max) {min_.y = min; max_.y = max;}

        
        /**
         * Takes 
         * 
        */
        point<FLOAT_T> map_to_cartesian_plane(int x, int y) {
            return {
                min_.x + static_cast<FLOAT_T>(x*(max_.x - min_.x))/(x_pixels_-1), 
                min_.y + static_cast<FLOAT_T>(y*(max_.y - min_.y))/(y_pixels_-1)
            };
        }
    };
}

#endif
