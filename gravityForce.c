/* Gravity force of Earth orbit and an object with rotation ,
   Study the law of physics ,
   OpenGL ,
   Modified by NG WING KIN , BsCE, MIEEE,MIET , OUHK , 12-5-2012 , 
   MSVC++ 2008 Express 
   Windows 7 SP1, Fujtisu Notebook LifeBook SH761 , Core i7-2620M , 2.7GHz, 8GB RAM, OS 64-bit.
*/

#include "Solar.h"   
#include "SimpleLight.h"
#include <stdlib.h> 
#include <stdio.h>
#include <stdarg.h>
#include <math.h>  	// For math routines 
#include <windows.h>
#include <GL/glut.h>	// OpenGL Graphics Utility Library

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  //  Avoid showing up the console window
GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;

void printw (float x, float y, float z, char* format, ...);    //  printf prints to file. printw prints to window

GLfloat X = 0.0f;            // Translate screen to x direction (left or right)
GLfloat Y = 0.0f;           // Translate screen to y direction (up or down)
GLfloat Z = 0.0f;           // Translate screen to z direction (zoom in or out)
GLfloat rotX = 0.0f;       // Rotate screen on x axis 
GLfloat rotY = 0.0f;       // Rotate screen on y axis
GLfloat rotZ = 0.0f;       // Rotate screen on z axis
GLfloat rotLx = 0.0f;     // Translate screen by using the glulookAt function (left or right)
GLfloat rotLy = 0.0f;    // Translate screen by using the glulookAt function (up or down)
GLfloat rotLz = 0.0f;    // Translate screen by using the glulookAt function (zoom in or out)

static GLenum spinMode = GL_TRUE;
static GLenum singleStep = GL_FALSE;
float xForce = 0.0;                                           // force

float xPosition = 0.0;
float xr=6.31874 * (float) pow(10.0,6);                     // radius
float Mass = 0.25 * 5.9768 * (float) pow(10.0,24);          // mass
float Gravity = 6.6742 * (float) pow(10.0,-11);             // gravity force constant 
float acceleration = ( Gravity * 5.9768 * (float) pow(10.0,24) ) / pow(xr,2);
float velocity = 19.6;
float xxposition = 0.0;

// These three variables control the animation's state and speed
static float CirclePerSec = 0.0;
static float AnimateIncrement = 60.0;  // Time step for animation (sec)
float Noemit[4] = {0.0, 0.0, 0.0, 1.0};
float Lt0spec[4] = {1.0, 1.0, 1.0, 1.0};
float zeroPos[4] = {0, 0, 0, 1};
float SphShininess = 50;	// Specular exponent for the spheres

const int TIMERMSECS = 20;
float animation_time = 0;
const float  animation_step = .5;

float SphAmbDiff[7][4] = {		        // The ambient/diffuse colors of the spheres 
	{0.5, 0.0, 0.0, 1.0},			// Red
	{0.5, 0.5, 0.0, 1.0},			// Yellow
	{0.0, 0.5, 0.0, 1.0},			// Green
	{0.0, 0.5, 0.5, 1.0},			// Cyan
	{0.0, 0.0, 0.5, 1.0},			// Blue
	{0.5, 0.0, 0.5, 1.0},			// Magenta
	{0.5, 0.2, 0.1, 0.9},                  // BROWN
};
float SphSpecular[4] = { 1, 1, 1, 1.0 };	

// Lighting values
float ambientLight[4] = {0.2, 0.2, 0.2, 1.0};
float Lt0amb[4] = {0.3, 0.3, 0.3, 1.0};
float Lt0diff[4] = {1.0, 1.0, 1.0, 1.0};

// glutKeyboardFunc is called below to set this function to handle
void printw (float x, float y, float z, char* format, ...)
{
	va_list args;	        //  Variable argument list
	int len;		//	String length
	int i;			//  Iterator
	char * text;	       // Text

	//  Initialize a variable argument list
	va_start(args, ormat);

	//  Return the number of characters in the string referenced the list of arguments
	//  _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1; 

	//  Allocate memory for a string of the specified size
	text =(char*) malloc(len * sizeof(char));
        if (text==NULL)
		exit (1);

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list 
	va_end(args);

	//  Specify the raster position for pixel operations
	glRasterPos3f (x, y, z);

	//  Draw the characters one by one
        for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

	//  Free the allocated memory for the string
	free(text);
}

// all normal key presses  
static void KeyPressFunc( unsigned char Key, int x, int y )
{
	switch ( Key )
{
	case 'R':
	case 'r':
		Key_r();
		break;
	case 's':
	case 'S':
		Key_s();
		break;
	case 27:	       // Escape key
		exit(1);
	case 'd':
	case 'D':
		Key_up();     // Double the animation time step
		break;
	case 'u':
	case 'U':          
		 Key_down(); // Halve the animation time step
		 break;

 case 'x': // Rotates screen on x axis 
    rotX -= 0.5f;
    break;
    case 'X': 			// Opposite way 
    rotX += 0.5f;
    break;
    case 'y': 			// Rotates screen on y axis
    rotY -= 0.5f;
    break;
    case 'Y': 			// Opposite way
    rotY += 0.5f; 
    break; 
    case 'z': 			// Rotates screen on z axis
    rotZ -= 0.5f;
    break;
    case 'Z': 			// Opposite way
    rotZ += 0.5f;
    break;
    // j,J,k,K,l,L uses the gluLookAt function for navigation
    case 'j':
    rotLx -= 0.2f; 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'J':
    rotLx += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break; 
    case 'k':
    rotLy -= 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'K':
    rotLy += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'l': 	        // It has a special case when the rotLZ becomes 
		       // less than -15 the screen is viewed from the opposite side
                      // therefore this if statement below does not allow rotLz be less than -15
    if(rotLz + 14 >= 0)
    rotLz -= 0.2f;           
    glMatrixMode(GL_MODELVIEW);    
    glLoadIdentity();  
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'L':
    rotLz += 0.2f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    break;
    case 'b':			 // Rotates on x axis by -90 degree
    rotX -= 90.0f;
    break;
    case 'B':			// Rotates on y axis by 90 degree
    rotX += 90.0f; 
    break;
    case 'n':			// Rotates on y axis by -90 degree
    rotY -= 90.0f;
    break;
    case 'N':			// Rotates on y axis by 90 degree
    rotY += 90.0f;
    break;
    case 'm':			// Rotates on z axis by -90 degree
    rotZ -= 90.0f; 
    break;
    case 'M':			// Rotates on z axis by 90 degree
    rotZ += 90.0f;
    break;
    case 'o':			// Default, resets the translations vies from starting view
    case 'O': 
    X = Y = 0.0f;
    Z = 0.0f;
    rotX = 0.0f;
    rotY = 0.0f;
    rotZ = 0.0f;
    rotLx = 0.0f;
    rotLy = 0.0f;
    rotLz = 0.0f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(rotLx, rotLy, 15.0f + rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);  
    break;
}
    glutPostRedisplay(); // Redraw the scene	

}

// glutSpecialFunc is called below to set this function to handle
//		all special key presses.  See glut.h for the names of
//		special keys.
static void SpecialKeyFunc( int Key, int x, int y )
{
	switch ( Key )
	{
//	case GLUT_KEY_UP:		
//		Key_up();
//		break;
//	case GLUT_KEY_DOWN:
//		Key_down();
//		break;
 case GLUT_KEY_LEFT : 		// Rotate on x axis
    X -= 0.1f;
    break;
    case GLUT_KEY_RIGHT :	 // Rotate on x axis (opposite)
    X += 0.1f;
    break;
    case GLUT_KEY_UP : 		// Rotate on y axis 
    Y += 0.1f;

    break;
    case GLUT_KEY_DOWN :	 // Rotate on y axis (opposite)
	
    Y -= 0.1f;
    break; 
    case GLUT_KEY_PAGE_UP: 	// Rotate on z axis
    Z -= 0.1f;
    break;
    case GLUT_KEY_PAGE_DOWN:	// Rotate on z axis (opposite)
    Z += 0.1f;
    break;
}
    glutPostRedisplay(); 	// Redraw the scene


}


static void Key_r(void)
{
	if ( singleStep )
	{					// If ending single step mode
		singleStep = GL_FALSE;
		spinMode = GL_TRUE;		// Restart animation
	}
	else 
	{
		spinMode = !spinMode;	        // Toggle animation on and off.
	}
}

static void Key_s(void)
{
	singleStep = GL_TRUE;
	spinMode = GL_TRUE;
}

static void Key_up(void)
{
    AnimateIncrement *= 2.0;			// Double the animation time step
}

static void Key_down(void)
{
    AnimateIncrement /= 2.0;			// Halve the animation time step
	
}

/*
 * Animate() handles the animation and the redrawing of the
 *		graphics window contents
 */


void animate(int t)
{
        float time_elapsed = TIMERMSECS/1000.0;
        float current_step = animation_step* time_elapsed;      
        if(current_step < animation_step*2) 
        animation_time += current_step;
        glutPostRedisplay();
        glutTimerFunc(TIMERMSECS, animate, 0);
}

static void Animate(void)

{
float x= 0.0;	
float y= 0.0;
float z= 0.0;

animate(0);

    if (spinMode)
	{
	    // Update the animation state
 	    CirclePerSec += AnimateIncrement/60.0;
	    CirclePerSec = CirclePerSec - ((int)(CirclePerSec/360))*360;  // one rotation
	   
            xForce =((float) cos(CirclePerSec) * Gravity * Mass) / pow(xr,2); 
            // equation  , f=cos(x) m1m2/r^2 
	
	    xPosition =(-( acceleration * pow(CirclePerSec,2) ) / 2 ) + (CirclePerSec * velocity); 
            //  p(t) = - a t2  + t v
	    xxposition = xPosition / 10000.0;   // scale down
	}

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear the redering window

 glPushMatrix(); 					// It is important to push the Matrix before calling 
 							// glRotatef and glTranslatef
    glRotatef(rotX,1.0,0.0,0.0);   // Rotate on x
    glRotatef(rotY,0.0,1.0,0.0);   // Rotate on y
    glRotatef(rotZ,0.0,0.0,1.0);   // Rotate on z
    glTranslatef(X, Y, Z); 	   // Translates the screen left or right, 
			           // up or down or zoom in zoom out
                                   // Draw the positive side of the lines x,y,z
    glBegin(GL_LINES);
    glColor3f (0.0, 1.0, 0.0);    // Green for x axis
    glVertex3f(0,0,0);
    glVertex3f(10,0,0);
    glColor3f(1.0,0.0,0.0);      // Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,10,0);
    glColor3f(0.0,0.0,1.0);      // Blue for z axis
    glVertex3f(0,0,0); 
    glVertex3f(0,0,10);
	
    glEnd();

 // Dotted lines for the negative sides of x,y,z
    glEnable(GL_LINE_STIPPLE); 	// Enable line stipple to use a 
			        // dotted pattern for the lines
    glLineStipple(1, 0x0101); 	// Dotted stipple pattern for the lines
    glBegin(GL_LINES); 
    glColor3f (0.0, 1.0, 0.0); 	// Green for x axis
    glVertex3f(-10,0,0);
    glVertex3f(0,0,0);
    glColor3f(1.0,0.0,0.0); 	// Red for y axis
    glVertex3f(0,0,0);
    glVertex3f(0,-10,0);
    glColor3f(0.0,0.0,1.0); 	// Blue for z axis
    glVertex3f(0,0,0);
    glVertex3f(0,0,-10);
    glEnd();
    glDisable(GL_LINE_STIPPLE); // Disable the line stipple
	
        glPushMatrix();				                 // Save matrix state
	glLoadIdentity();					// Clear the current matrix (Modelview)
	glTranslatef ( 0.0, 0.0, -10.0 );   
       // Back off eight units to be able to view from the origin.
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, SphAmbDiff[2] );  
      // select color array sphambdiff[] 
 	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SphSpecular );
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, SphShininess);	
    glTranslatef( 0.0, 0.0, 0.0);
    glRotatef( CirclePerSec, 0.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Lt0spec);	// Make sphere glow (emissive)
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Noemit);	// Turn off emission
 //  glLightfv(GL_LIGHT0, GL_POSITION, zeroPos );
 
   glRotatef( CirclePerSec, 0.0, 1.0, 0.0);		       // object 1 , Earth
   glTranslatef(2.0, 0.0, 0.0 );
   glutSolidSphere (0.5, 30, 30);

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, SphAmbDiff[3] );  
   // select color array sphambdiff[] 
   glRotatef( CirclePerSec, xxposition, 1.0, 0.0 );           // object 2 
   glTranslatef(4.0, 0.0, 0.0 );
   glutSolidSphere (0.2, 30, 30);

glPopMatrix();
printw (-13.0, -6.0, 0, "Velocityy = %f", velocity);
printw (-1.0, -6.0,0, "Acceleration = %f", acceleration);
printw (-13.0, -7.0,0, "Angle  = %f", CirclePerSec);
printw (-1.0, -7.0,0, "Position = %f", xxposition); 
printw (-13.0, -8.0,0, "Force  = %f", xForce);
printw (-1.0,-8.0,0, "Current time = %f",  animation_time);

glFlush();				// Flush the pipeline, and swap the buffers
glutSwapBuffers();
 
	if ( singleStep )
	{
		spinMode = GL_FALSE;
	}

 

	glutPostRedisplay();		// Request a re-draw for animation purposes

}

 
// Initialize OpenGL's rendering modes
void OpenGLInit(void)
{

    glClearColor (0.0, 0.0, 0.0, 0.0);           // Clear the color 
    glShadeModel( GL_FLAT );			// Set the shading model to GL_FLAT
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable (GL_LINE_SMOOTH);   
 //   glEnable( GL_DEPTH_TEST );
 glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Set Line Antialiasing

}

// ResizeWindow is called when the window is resized
static void ResizeWindow(int w, int h)
{
    float aspectRatio;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport( 0, 0, w, h );	// View port uses whole window
	aspectRatio = (float)w/(float)h;

    // Set up the projection view matrix (not very well!)
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0, aspectRatio, 1.0, 30.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );

// glViewport (0, 0, (GLsizei) w, (GLsizei) h); // Set the viewport
//   glMatrixMode (GL_PROJECTION); 	// Set the Matrix mode
//   glLoadIdentity (); 
//   gluPerspective(75, (GLfloat) w /(GLfloat) h , 0.10, 100.0);
//  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt (rotLx, rotLy, 15.0 + rotLz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 

}

// Initialize OpenGL's rendering modes
void initRendering()
{
   

	glEnable( GL_DEPTH_TEST );	// Depth testing must be turned on

	glEnable(GL_LIGHTING);		// Enable lighting calculations
	glEnable(GL_LIGHT0);		// Turn on light #0.

                                 	// Set global ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);	

	// Light 0 light values.  Its position is set in drawScene().
	glLightfv(GL_LIGHT0, GL_AMBIENT, Lt0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Lt0spec);

}


// Main routine    
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{

     Beep(1500,200); // system beep sound

    // Need to double buffer for animation
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 600, 360 );
    glutCreateWindow( "Gravity Force Simulation of Earth Orbit");
	

    // Initialize OpenGL.
    OpenGLInit();
    initRendering();  			// turn on rendering function
					// Set up callback functions for key presses
    glutKeyboardFunc( KeyPressFunc );
    glutSpecialFunc( SpecialKeyFunc );
					// Set up the callback function for resizing windows
    glutReshapeFunc( ResizeWindow );
					// Callback for graphics image redrawing
    glutDisplayFunc( Animate );
					// Start the main loop.  glutMainLoop never returns


    glutMainLoop(  );
    return(0);			// Compiler requires this to be here
}
