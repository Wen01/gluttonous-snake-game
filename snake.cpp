/*
CS 349 A1 Skeleton Code - Snake

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o snake snake.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./snake

Note: the -L option and -lstdc++ may not be needed on some machines.
*/

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <sstream>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
int FPS = 30;
const int width = 800;
const int height = 600;
int inputSpeed = 5;


#define North  1
#define South  2
#define East  3
#define West  4

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
    Pixmap   buffer;
	GC		 gc[3];
	int		width;		// size of window
	int		height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}

struct body {
	int x;
	int y;
	int direction;

	body (int x, int y, int direction): x(x), y(y), direction(direction){}
};


/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};


class Obstacle : public Displayable {
    int X;
    int Y;
    int length;
    int direction;

public:
    Obstacle (int x, int y, int length, int direction): X(x), Y(y), length(length), direction(direction){}

    int getX () {return X;}
    int getY () {return Y;}
    int getDir () {return direction;}
    int getLen () {return length;}

    virtual void paint (XInfo &xinfo) {
        if (direction == South) {
            for (int i = 0; i < length; i++) {
                XDrawRectangle (xinfo.display, xinfo.buffer, xinfo.gc[0], X, Y + i*BufferSize, BufferSize, BufferSize);
            }
        }
        if (direction == East) {
            for (int i = 0; i < length; i++) {
                XDrawRectangle (xinfo.display, xinfo.buffer, xinfo.gc[0], X+i*BufferSize, Y, BufferSize, BufferSize);
            }
        }


    }
};

class Snake : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			vector<body*>::iterator it = Body.begin();
			for (it = Body.begin(); it != Body.end(); it++) {
				XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc[0], (*it)->x + 1, (*it)->y + 1, blockSize-1, blockSize-1);
			}
		}
		
		void move(XInfo &xinfo, bool &gameOver, bool &gamePause, list<Obstacle *> &blocks ) {
            // ** ADD YOUR LOGIC **
            // Here, you will be performing collision detection between the snake, 
            // the fruit, and the obstacles depending on what the snake lands on.
			for (vector<body*>::iterator it = Body.begin(); it != Body.end(); it++) {
				int single_dir = (*it)->direction;
				if (single_dir == North) {
					(*it)->y -= blockSize;
				}
				else if (single_dir == South) {
					(*it)->y += blockSize;
				}
				else if (single_dir == East) {
					(*it)->x += blockSize;
				}
				else if (single_dir == West) {
					(*it)->x -= blockSize;
				}
                // hit its own body
                if (Body[0]->x == (*it)->x && Body[0]->y == (*it)->y && it != Body.begin()) {
                    didHitObstacle(gameOver, gamePause);
                }

			}

			int length = Body.size();
			for (int i = length-1; i>0; i--) {
				Body[i]->direction = Body[i-1]->direction;
			}
			// hit boundary
            for (list<Obstacle *>::iterator block = blocks.begin(); block != blocks.end(); block++) {
                if ((*block)->getDir() == South) {
                    if (Body[0]->y >= (*block)->getY() &&
                            Body[0]->y <= ((*block)->getY() + (*block)->getLen() * BufferSize) &&
                            (abs(Body[0]->x - (*block)->getX()) <= 9)) {
                        //cout << Body[0]->x << " " << (*block)->getX() << endl;
                        didHitObstacle(gameOver, gamePause);
                    }
                }

                if ((*block)->getDir() == East) {
                    if (Body[0]->x >= (*block)->getX() &&
                        Body[0]->x <= ((*block)->getX() + (*block)->getLen() * BufferSize) &&
                        (abs(Body[0]->y - (*block)->getY()) <= 9)) {
                        didHitObstacle(gameOver, gamePause);
                    }
                }
            }

            for (vector<body*>::iterator it = Body.begin(); it != Body.end(); it++) {
                if ((*it)->x == 0 && (*it)->direction == West) {
                    (*it)->x = 790;
                }
                if ((*it)->x == 800 && (*it)->direction == East) {
                    (*it)->x = 0;
                }
                if ((*it)->y == 0 && (*it)->direction == North) (*it)->y = 590;
                if ((*it)->y == 600 && (*it)->direction == South) (*it)->y = 0;
            }


		}
		
		int getX() {
			return Body[0]->x;
		}
		
		int getY() {
			return Body[0]->y;
		}

        /*
         * ** ADD YOUR LOGIC **
         * Use these placeholder methods as guidance for implementing the snake behaviour. 
         * You do not have to use these methods, feel free to implement your own.
         */ 
        void didEatFruit(int x, int y) {

			body *new_head = new body(x, y, direction);
			vector<body*>::iterator it = Body.begin();
			Body.insert(it, new_head);

			score += 5;
        }

        void didHitObstacle(bool &gameOver, bool &gamePause) {
            gameOver = true;
			gamePause = true;
			//cout << "Game over!" << endl;
			//cout << "Press Q to quit, R to restart game." << endl;
        }

		bool checkFruit (int f_x, int f_y) {
			if (Body[0]->x == f_x && Body[0]->y + blockSize == f_y
					&& direction == South) {
				didEatFruit(f_x, f_y);
				return true;
			}
			if (Body[0]->x == f_x && Body[0]->y - blockSize == f_y
					&& direction == North) {
				didEatFruit(f_x, f_y);
				return true;
			}
			if (Body[0]->x + blockSize == f_x && Body[0]->y == f_y
					&& direction == East) {
				didEatFruit(f_x, f_y);
				return true;
			}
			if (Body[0]->x - blockSize == f_x && Body[0]->y == f_y
					&& direction == West) {
				didEatFruit(f_x, f_y);
				return true;
			}
			else return false;
		}

		int getDirection () {
			return Body[0]->direction;
		}

        void turnLeft() {
			if (direction == North) {
				Body[0]->direction = West;
			}
			else if (direction == South) {
				Body[0]->direction = East;
			}
			else if (direction == East) {
				Body[0]->direction = North;
			}
			else {
				Body[0]->direction = South;
			}
			direction = Body[0]->direction;

        }

        void turnRight() {
			if (direction == North) {
				Body[0]->direction = East;
			}
			else if (direction == South) {
				Body[0]->direction = West;
			}
			else if (direction == East) {
				Body[0]->direction = South;
			}
			else {
				Body[0]->direction = North;
			}
			direction = Body[0]->direction;
        }
		void setSpeed (int s) {speed = s;}
		int getSpeed () {return speed;}
		int getScore() {return score;}
		
		Snake(int x, int y) {
			direction = East;
            blockSize = 10;
			this->speed = inputSpeed;
			for (int i = 0; i < 3; i++) {
				body* b = new body(x - i*blockSize, y, direction);
				Body.push_back(b);
			}
			//turn = false;
			score = 0;
		}

		void reset (int x, int y) {
			for (vector<body*>::iterator it = Body.begin(); it != Body.end(); it++) {
				delete(*it);
			}
			Body.clear();

			direction = East;
			blockSize = 10;
			this->speed = inputSpeed;
			for (int i = 0; i < 3; i++) {
				body* b = new body(x - i*blockSize, y, direction);
				Body.push_back(b);
			}

			score = 0;
		}

		~Snake () {
			for (vector<body*>::iterator it = Body.begin(); it != Body.end(); it++) {
				delete(*it);
			}
			Body.clear();
		}
	
	private:
		vector <body*> Body;
		int blockSize;
		int direction;
		int speed;
		int score;
		//bool turn;
};

class Fruit : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc[0], x, y, 9, 9);
        }

        Fruit() {
            // ** ADD YOUR LOGIC **
            // generate the x and y value for the fruit 
            x = (rand() % 75 + 3) * 10;
            y = (rand() % 58 + 1) * 10;
        }

        // ** ADD YOUR LOGIC **
        /*
         * The fruit needs to be re-generated at new location every time a snake eats it. See the assignment webpage for more details.
         */
        bool onBoundary (list<Obstacle*> &blocks) {
            for (list<Obstacle *>::iterator block = blocks.begin(); block != blocks.end(); block++) {
                if ((*block)->getDir() == South) {
                    if (y >= (*block)->getY() &&
                        y <= ((*block)->getY() + (*block)->getLen() * BufferSize) &&
                        (x == (*block)->getX() || x == (*block)->getX() + 9)) {
                        return true;
                    }
                }

                if ((*block)->getDir() == East) {
                    if (x >= (*block)->getX() &&
                        x <= ((*block)->getX() + (*block)->getLen() * BufferSize) &&
                        (y == (*block)->getY() || y == (*block)->getY() + 9)) {
                        return true;
                    }
                }
            } return false;
        }

		void changeLocation (list<Obstacle*> &blocks) {
			x = (rand() % 75 + 3) * 10;
			y = (rand() % 58 + 1) * 10;

            while (onBoundary(blocks)) {
                x = (rand() % 75 + 3) * 10;
                y = (rand() % 58 + 1) * 10;
            }

		}

		int getX () {
			return x;
		}
		int getY() {
			return y;
		}

    private:
        int x;
        int y;
};


list<Displayable *> dList;           // list of Displayables
Snake snake(100, 450);
Fruit fruit;

Obstacle block1 (300, 200, 10, South);
Obstacle block2 (500, 200, 10, South);
Obstacle block3 (350, 150, 10, East);
Obstacle block4 (350, 350, 10, East);
Obstacle block5 (0, 50, 50, South);
Obstacle block6 (790, 50, 50, South);
Obstacle block7 (50, 0, 70, East);
Obstacle block8 (50, 590, 70, East);



list<Obstacle *> blocks;


bool gameOver = false;
bool gameStart = false;
bool gamePause = true;

/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

    i = 1;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                       1, LineSolid, CapButt, JoinRound);

    int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
    xInfo.buffer = XCreatePixmap(xInfo.display, xInfo.window, hints.width, hints.height, depth);
    xInfo.width = hints.width;
    xInfo.height = hints.height;


    XFontStruct * font;
    font = XLoadQueryFont (xInfo.display, "12x24");
    XSetFont (xInfo.display, xInfo.gc[0], font->fid);
    XSetFont (xInfo.display, xInfo.gc[1], font->fid);



	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask);  // for resize events


	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);

}

void pausePrint (XInfo &xinfo) {
    string info = "Start Game";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 50, info.c_str(), info.length());
    info = "[S] Start";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 80, info.c_str(), info.length());
    info = "[W][A][S][D] Move";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 280, 110, info.c_str(), info.length());
    info = "[P] Pause";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 140, info.c_str(), info.length());
    info = "[C] Continue";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 170, info.c_str(), info.length());
    info = "[R] Restart";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 200, info.c_str(), info.length());
    info = "[Q] Quit";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 230, info.c_str(), info.length());
    info = "Name: Linyi Wen";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 600, 500, info.c_str(), info.length());
    info = "ID: 20600789";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 600, 550, info.c_str(), info.length());
}

void overPrint (XInfo &xinfo) {
    string info = "Game Over!";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 330, 140, info.c_str(), info.length());
    stringstream ss;
    ss << "Your score is ";
    ss << snake.getScore();
    info = ss.str();
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 310, 170, info.c_str(), info.length());
    info = "Press R to restart, Q to quit";
    XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 260, 200, info.c_str(), info.length());
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

    XFillRectangle(xinfo.display, xinfo.buffer, xinfo.gc[1],
                   0, 0, xinfo.width, xinfo.height);

    XWindowAttributes windowInfo;
    XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
    unsigned int height = windowInfo.height;
    unsigned int width = windowInfo.width;
	
	// get height and width of window (might have changed since last repaint)
    if (gameStart && !gamePause) {
        stringstream ss;
        ss << "Score: ";
        ss << snake.getScore();
        string info = ss.str();
        XDrawString(xinfo.display, xinfo.buffer, xinfo.gc[0], 550, 50, info.c_str(), info.length());
    //XCopyArea(xinfo.display, xinfo.buffer, xinfo.window, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height, 0, 0);
    // big black rectangle to clear background

    // draw display list
        while (begin != end) {
            Displayable *d = *begin;
            d->paint(xinfo);
            //XCopyArea(xinfo.display, xinfo.buffer, xinfo.window, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height, 0, 0);
            begin++;
        }
    }

    else if (gameOver) {
        overPrint(xinfo);
    }
    else if (!gameStart) {
        pausePrint(xinfo);
    }

    else if (gamePause) pausePrint(xinfo);

	XFlush( xinfo.display );
}


void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		//printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			cout << "Your score is " << snake.getScore() << endl;
			error ("Terminate normally.");
		}
		else if (text[0] == 'w') {
			if (snake.getDirection() == East) snake.turnLeft();
			else if (snake.getDirection() == West) snake.turnRight();
		}
		else if (text[0] == 'a') {
			if (snake.getDirection() == North) snake.turnLeft();
			else if (snake.getDirection() == South) snake.turnRight();
		}
		else if (text[0] == 's') {
            if (!gameStart) {
                gameStart = true;
                gamePause = false;
            }
            else {
			    if (snake.getDirection() == East) snake.turnRight();
			    else if (snake.getDirection() == West) snake.turnLeft();
            }
		}
		else if (text[0] == 'd') {
			if (snake.getDirection() == North) snake.turnRight();
			else if (snake.getDirection() == South) snake.turnLeft();
		}
		else if (text[0] == 'r') {
			if (gamePause || gameOver) {
				snake.reset(100, 450);
				fruit.changeLocation(blocks);
				gamePause = false;
                gameOver = false;
			}
			else {
				gamePause = true;
				cout << "Do you want to restart the game?" << endl;
				cout << "Press R to restart, P to continue." << endl;
			}
		}
		else if (text[0] == 'p') {
		    gamePause = 1;
		}
        else if (text[0] == 'c') {
            if (gamePause) gamePause = false;
        }
	}
}

void handleAnimation(XInfo &xinfo, int inside) {
    /*
     * ADD YOUR OWN LOGIC
     * This method handles animation for different objects on the screen and readies the next frame before the screen is re-painted.
     */
	while (snake.checkFruit(fruit.getX(), fruit.getY())) {
		fruit.changeLocation(blocks);
	}
	snake.move(xinfo, gameOver, gamePause, blocks);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
    dList.push_front(&block1);
    dList.push_front(&block2);
    dList.push_front(&block3);
    dList.push_front(&block4);
    dList.push_front(&block5);
    dList.push_front(&block6);
    dList.push_front(&block7);
    dList.push_front(&block8);

    blocks.push_front(&block1);
    blocks.push_front(&block2);
    blocks.push_front(&block3);
    blocks.push_front(&block4);
    blocks.push_front(&block5);
    blocks.push_front(&block6);
    blocks.push_front(&block7);
    blocks.push_front(&block8);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	unsigned long lastMove = 0;
	int inside = 0;



	while( true ) {
		/*
		 * This is NOT a performant event loop!  
		 * It needs help!
		 */
		
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			//cout << "event.type=" << event.type << "\n";
			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		} 

		unsigned long end = now();

		if (end - lastRepaint > 1000000/FPS) {
			repaint(xinfo);
            XCopyArea(xinfo.display, xinfo.buffer, xinfo.window, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height, 0, 0);
            lastRepaint = now();
		}
        if (now() - lastMove > 1000000/inputSpeed && !gamePause && !gameOver && gameStart) {
            handleAnimation(xinfo, inside);
            lastMove = now();
        }

		if (XPending(xinfo.display) == 0) {
			usleep(1000000 / FPS - (end - lastRepaint));
		}
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

	int test = "Hello World";
	if (argc == 3) {
		FPS = atoi(argv[1]);
        inputSpeed = atoi(argv[2]);
		snake.setSpeed(inputSpeed);
	}

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
