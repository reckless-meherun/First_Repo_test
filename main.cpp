//g++ main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf && ./a.out

//Using SDL, SDL_image, standard IO, vectors, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <bits/stdc++.h>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1578;
const int SCREEN_HEIGHT = 878;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, double angle, SDL_RendererFlip flip, SDL_Rect* clip = NULL,  SDL_Point* center = NULL );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 5;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the dot on the screen
		void render();
	

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;
};

//Walking animation
		const int WALKING_ANIMATION_FRAMES = 4;
		SDL_Rect gDotClips[ WALKING_ANIMATION_FRAMES ];

class Animal
{
    public:
		//The dimensions of the dot
		static const int Animal_WIDTH = 20;
		static const int Animal_HEIGHT = 20;

		//Initializes the variables
		Animal();

		//Shows the dot on the screen
		void render();

    private:
		//The X and Y offsets of the dot
		int posAx1, posAx2, posAx3, posAy1, posAy2, posAy3;

		//The velocity of the dot
		int mVelX, mVelY;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gBGTexture;
LTexture gAnimalTexture; 



LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;			
			//mWidth = 100;
			mHeight = loadedSurface->h;
			//mHeight = 100;
			// gDotTexture.mHeight = 100;
			// gDotTexture.mWidth = 70;
			// gAnimalTexture.mHeight = 200;
			// gAnimalTexture.mWidth = 140;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, double angle, SDL_RendererFlip flip, SDL_Rect* clip, SDL_Point* center)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip ); //this is just like SDL_RenderCopy with some additional stuff
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{	
	return mHeight;
}

Dot::Dot()
{
    //Initialize the offsets
    mPosX = 0;
    mPosY = SCREEN_HEIGHT/2;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::render()
{
    //Show the dot
	gDotTexture.render( mPosX, mPosY, 0.0, SDL_FLIP_NONE);
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: 
            //gDotTexture.render( mPosX, mPosY, 0.0, SDL_FLIP_VERTICAL);
            mVelX -= DOT_VEL; 
            
            break;
            case SDLK_RIGHT: mVelX += DOT_VEL; 
			break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: 
            //gDotTexture.render( mPosX, mPosY, 0.0, SDL_FLIP_VERTICAL);
            mVelX += DOT_VEL;  
            break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }

}

void Dot::move()
{
    //Move the dot left or right
    mPosX += mVelX;

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

    //Move the dot up or down
    mPosY += mVelY;

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY -= mVelY;
    }
}



Animal::Animal()
{
    posAx1 = SCREEN_WIDTH/4;
    posAx2 = SCREEN_WIDTH/2;
    posAx3 = SCREEN_WIDTH*3/4; 
    posAy1 = 685;
    posAy2 = 685;
    posAy3 = 685;
}

void Animal::render()
{
	//Show the animal
	gAnimalTexture.render( posAx1, posAy1, 0, SDL_FLIP_NONE );
    gAnimalTexture.render( posAx2, posAy2, 0, SDL_FLIP_NONE );
    gAnimalTexture.render( posAx3, posAy3, 0, SDL_FLIP_NONE );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile("shorted_sprite_sheet.png") )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	else
	{
		//Set Dot clips
		gDotClips[ 0 ].x =   0;
		gDotClips[ 0 ].y =   0;
		gDotClips[ 0 ].w = 112;
		gDotClips[ 0 ].h = 166;

		gDotClips[ 1 ].x =  112;
		gDotClips[ 1 ].y =   0;
		gDotClips[ 1 ].w =  112;
		gDotClips[ 1 ].h = 166;
		
		gDotClips[ 2 ].x = 224;
		gDotClips[ 2 ].y =   0;
		gDotClips[ 2 ].w = 112;
		gDotClips[ 2 ].h = 166;

		gDotClips[ 3 ].x = 336;
		gDotClips[ 3 ].y =   0;
		gDotClips[ 3 ].w = 112;
		gDotClips[ 3 ].h = 166;
	}

	//load animals
	if( !gAnimalTexture.loadFromFile("animal.png") )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	//Load background texture
	if( !gBGTexture.loadFromFile( "photo.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();
	gBGTexture.free();
	gAnimalTexture.free();
	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if(!loadMedia())
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot dot;

			//Current animation frame
			int frame = 0;

			//animal
			Animal animal;
			//The background scrolling offset
			int scrollingOffset = 0;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
                    
				}

				

				//Scroll background
				--scrollingOffset;
				if( scrollingOffset < -gBGTexture.getWidth() )
				{
					scrollingOffset = 0;
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render background
				gBGTexture.render( scrollingOffset, 0, 0, SDL_FLIP_NONE );
				gBGTexture.render( scrollingOffset + gBGTexture.getWidth(), 0, 0, SDL_FLIP_NONE );

				//Render objects
				//Render current frame
				SDL_Rect* currentClip = &gDotClips[ frame / 4 ];
				gDotTexture.render( ( SCREEN_WIDTH - currentClip->w ) / 3, 579, 0, SDL_FLIP_NONE, currentClip );
		
				//Go to next frame
				++frame;
				
				
				
				//Cycle animation
				if( frame / 4 >= WALKING_ANIMATION_FRAMES )
				{
					frame = 0;
				}		
				
				//gDotClips[ frame ].x++;

				//render animals
				animal.render();

				//Update screen
				SDL_RenderPresent( gRenderer );

				SDL_Delay(15);
			}
		}
	}
	//Free resources and close SDL
	close();
	return 0;
}