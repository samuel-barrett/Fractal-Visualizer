To build this project, a script (or sequence of commands) similar to below can be used
where TOP_DIR is the top level directory of the project, and 

TOP_DIR=[TOP DIRECTORY PATH]/cpp_project-fractal-visualizer
INSTALL_DIR=[INSTALL PATH]/home/jdoe/INSTALL_DIR


cd $TOP_DIR
cmake -H. -Btmp_cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
cmake --build tmp_cmake --clean-first --target install

#To run the project, the following command can then be used
$INSTALL_DIR/bin/demo


Various command line arguments can be used to control certain parameters on the screen
	-h [height]	Specify window height in pixels
	-w [width]	Specify window width  in pixels
	-a [alpha]	Specify henon alpha (or a) value
	-b [beta]	Specify henon beta (or b) value
	-t [threshold]	Specify value at which point the point is assumed to have escaped

	-L [leftmost point],[lowest point]:
		Specify bottom left point to display initially on xy plane
	-U [rightmost point],[highest point]:
		Specify top right point to display initially on xy plane
	-f [fractal]:
		Specify top right point to display initially on xy plane
		Arguments:
			mandelbrot: display mandelbrot set
			henon: display henon (default)


Mouse/Keyboard Interaction:
    Once the GUI has successfull been opened, various actions can be used to interact with the application

    Mouse translate:
        To pan, select a point on the GUI and hold down the left mouse key. Move the mouse and release
        to pan view different regions of the fractal

    Mouse select:
        You can select a region of the screen to zoom into using the right mouse key by pressing down on one corner
        which where you would like to select and releasing at another. The view will be zoomed to that scale, but
        the aspect ratio ratio will be preserved to avoid squishing the fractal view excessively.
    
    Zoom:
        To zoom in: on the center of the screen the z key can be pressed. You can also use the mouse scroll wheel 
        and scroll up.
 
        To zomm out: the x key can be pressed, can also use mouse scroll wheel down.

    Resizing:
        Resizing the window result in an proportional increase in the viewing region.

    Reset viewing region:
        To reset the viewing region to the initial paramters, press the r/R key
    
    Program termination:
        One way to terminate the program is to hit the escape key
