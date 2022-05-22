#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#pragma warning(disable:4996)
extern "C" 
{
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME (1000 / SCREEN_FPS)
#define SPEED 10.0
#define MAX_B 7
#define MAX_G 2
#define BLOCK_FREQUENCY (SCREEN_WIDTH/2)
#define	LAST_BLOCK BLOCK_FREQUENCY*MAX_B
#define UNI_HEIGHT 25
#define UNI_WIDTH 35
#define B_HEIGHT 32
#define P_WIDTH 160
#define UNICORN_X UNI_WIDTH+5
#define UNICORN_Y (SCREEN_HEIGHT/2)
#define JUMP (SCREEN_HEIGHT/4)

struct Blocks
{
	int xpos;
	int ypos;
};

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);

// inicjalizacja sdl, stworzenie okna, ekranu itd.
bool initialization(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** screen, SDL_Texture** scrtex);
//tekst informacyny
void InfoText(SDL_Surface* screen, int red, int blue, char  text[128], double worldTime, double fps, SDL_Surface* charset);
//zaladowanie bmp
void loadmedia(SDL_Surface*& charset, SDL_Surface*& screen, SDL_Surface*& unicorn, SDL_Texture*& scrtex, SDL_Window*& window, SDL_Renderer*& renderer, SDL_Surface*& background, SDL_Surface*& block, SDL_Surface*& ground, SDL_Surface*& platform);
//sprawdzenie jaki ruch powinien wykonac jednorozec
void movecheck(bool mup, bool& falling, int canjump, double& ypos, double& fallingTimer, bool mleft, double& xpos, bool mright, double delta, int ison, int fallonblock, int fallonground);
// czyszczenie sdl i koniec programu
void programend(SDL_Surface** screen, SDL_Surface** charset, SDL_Surface** unicorn, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** background, SDL_Surface** block, SDL_Surface** ground, SDL_Surface** platform);
//sprawdzenie kolizji i pozycji wzgledem siebie wszystkich obiektow
void positioncheck(int& fallonblock, int& fallonground, bool& collision, int& ison, int& canjump, Blocks  b[], double xpos, double ypos, Blocks  g[]);
//sprawdzenie przekroczenia okna i pojawienia sie po drugiej stronie planszy
void outofwindowandmovecheck(bool collision, Blocks  g[], double xpos, double ypos, Blocks  b[]);
//liczenie czasu i fps
void timeandfps(int& t2, double& delta, int& t1, double& worldTime, double& fpsTimer, double& fps, int& frames);
//obsluga zdarzen
void eventspoll(SDL_Event& event, int& quit, bool& mup, bool& mleft, bool& mright, int& newgame);
// nowa gra
void newgamecreate(int* t1, int* frames, double* worldTime, double* fpsTimer, double* fps, double* distance, int* newgame, double* xpos, double* ypos, bool* falling, double* fallingTimer, Blocks b[], int* fallonblock, int* fallonground, bool* collision, Blocks g[], int* ison, int* canjump);
//pojawianie obiektow na ekranie
void drawingallsurfaces(SDL_Surface* screen, SDL_Surface* unicorn, SDL_Surface* block, Blocks  b[], SDL_Surface* platform, SDL_Surface* ground, Blocks  g[]);
//limit fps
void fpslimit(int t1);



// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv)
{
	bool mup = false, mleft = false, mright = false, falling = false, collision;
	int t1, t2, quit = 0, frames, newgame = 1, fallonblock, fallonground, ison, canjump;
	double delta, worldTime, fpsTimer, fps, distance, xpos, ypos, fallingTimer = 0;
	SDL_Event event;
	SDL_Surface* screen = NULL, * charset = NULL, * unicorn = NULL, * background = NULL, * block = NULL, * ground = NULL, * platform = NULL;
	SDL_Texture *scrtex = NULL;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	Blocks b[MAX_B], g[MAX_G];

	if (!initialization(&window, &renderer, &screen, &scrtex))
		exit(0);
	// wczytanie danych
	loadmedia(charset, screen, unicorn, scrtex, window, renderer, background, block, ground, platform);
	char text[128];
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00), red = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00), green = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00), blue = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	//GAME LOOP
	while(!quit)
	{
		if (newgame == 1)
			newgamecreate(&t1, &frames, &worldTime, &fpsTimer, &fps, &distance, &newgame, &xpos, &ypos, &falling, &fallingTimer, b, &fallonblock, &fallonground,&collision, g, &ison, &canjump);
		//liczenie czasu i fps
		timeandfps(t2, delta, t1, worldTime, fpsTimer, fps, frames);

		SDL_BlitSurface(background, NULL, screen, NULL);
		//przypisanie zmiennych odpowiedzialnych za ruch i sprawdzenie pozycji, kolizji itd
		movecheck(mup, falling, canjump, ypos, fallingTimer, mleft, xpos, mright, delta, ison, fallonblock, fallonground);
		positioncheck(fallonblock, fallonground, collision, ison, canjump, b, xpos, ypos, g);
		outofwindowandmovecheck(collision, g, xpos, ypos, b);

		drawingallsurfaces(screen, unicorn, block, b, platform, ground, g);
		// tekst informacyjny
		InfoText(screen, red, blue, text, worldTime, fps, charset);
		//wyswietlanie
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y)
		eventspoll(event, quit, mup, mleft, mright, newgame);

		frames++;
		fpslimit(t1);
	};
	// zwolnienie powierzchni
	programend(&screen, &charset, &unicorn, &scrtex, &window, &renderer, &background, &block, &ground, &platform);
}

	void fpslimit(int t1)
	{
		int fpscheck = SDL_GetTicks() - t1;
		if (fpscheck < SCREEN_TICKS_PER_FRAME)
		{
			//Wait remaining time
			SDL_Delay(SCREEN_TICKS_PER_FRAME - fpscheck);
		}
	}

	void drawingallsurfaces(SDL_Surface* screen, SDL_Surface* unicorn, SDL_Surface* block, Blocks  b[], SDL_Surface* platform, SDL_Surface* ground, Blocks  g[])
	{
		DrawSurface(screen, unicorn, UNICORN_X, UNICORN_Y);

		for (int i = 0; i < MAX_B; i++)
		{
			if (i < MAX_B / 2)
				DrawSurface(screen, block, b[i].xpos, b[i].ypos);
			else
				DrawSurface(screen, platform, b[i].xpos, b[i].ypos);
		}

		for (int i = 0; i < MAX_G; i++)
		{

			DrawSurface(screen, ground, g[i].xpos, g[i].ypos);
		}
	}

	void eventspoll(SDL_Event& event, int& quit, bool& mup, bool& mleft, bool& mright, int& newgame)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_UP)
				{
					mup = true;
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					mleft = true;
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					mright = true;
				}
				else if (event.key.keysym.sym == SDLK_n) newgame = 1;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_UP)
				{
					mup = false;
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					mleft = false;
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					mright = false;
				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
	}



	void timeandfps(int& t2, double& delta, int& t1, double& worldTime, double& fpsTimer, double& fps, int& frames)
	{
		t2 = SDL_GetTicks();
		delta = (t2 - t1) * 0.001;
		t1 = t2;
		worldTime += delta;
		fpsTimer += delta;
		if (fpsTimer > 0.5)
		{
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};
	}



	void outofwindowandmovecheck(bool collision, Blocks  g[], double xpos, double ypos, Blocks  b[])
	{
		if (collision == false)
		{
			for (int i = 0; i < MAX_G; i++)
			{
				if (g[i].xpos + xpos + BLOCK_FREQUENCY < 0)
				{
					g[i].xpos = BLOCK_FREQUENCY * 3 + xpos;
					g[i].ypos += ypos;
				}
				else if (g[i].xpos + xpos >= BLOCK_FREQUENCY * 3)
				{
					g[i].xpos = -BLOCK_FREQUENCY;
					g[i].ypos += ypos;
				}
				else
				{
					g[i].xpos += xpos;
					g[i].ypos += ypos;
				}
			}

			for (int i = 0; i < MAX_B; i++)
			{
				if (i < MAX_B / 2)
				{
					if (b[i].xpos + xpos + B_HEIGHT < 0)
					{
						b[i].xpos = LAST_BLOCK + 130 + xpos;
						b[i].ypos += ypos;
					}
					else if (b[i].xpos + xpos >= LAST_BLOCK + 130)
					{
						b[i].xpos = -B_HEIGHT;
						b[i].ypos += ypos;
					}
					else
					{
						b[i].xpos += xpos;
						b[i].ypos += ypos;
					}
				}
				else
				{
					if (b[i].xpos + xpos + P_WIDTH < 0)
					{
						b[i].xpos = LAST_BLOCK + xpos;
						b[i].ypos += ypos;
					}
					else if (b[i].xpos + xpos >= LAST_BLOCK)
					{
						b[i].xpos = -P_WIDTH;
						b[i].ypos += ypos;
					}
					else
					{
						b[i].xpos += xpos;
						b[i].ypos += ypos;
					}
				}

			}
		}
	}



	void positioncheck(int& fallonblock, int& fallonground, bool& collision, int& ison, int& canjump, Blocks  b[], double xpos, double ypos, Blocks  g[])
	{
		fallonblock = 0;
		fallonground = 0;
		collision = false;
		ison = -1;
		canjump = 1;
		for (int i = 0; i < MAX_B; i++)
		{
			if (i < MAX_B / 2)
			{//bloki
				if ((b[i].xpos + xpos - B_HEIGHT) < UNICORN_X + UNI_WIDTH && (b[i].xpos + xpos + B_HEIGHT) > UNICORN_X - UNI_WIDTH)//jesli pokrywa sie x blokow
				{
					if (b[i].ypos + ypos + B_HEIGHT > UNICORN_Y - UNI_HEIGHT && b[i].ypos + ypos - B_HEIGHT < UNICORN_Y + UNI_HEIGHT)//jesli pokrywa sie y blokow
					{
						collision = true;
					}
					else if (UNICORN_Y + UNI_HEIGHT <= b[i].ypos + ypos - B_HEIGHT)//jesli jednorozec jest nad tym blokiem
					{
						if (((b[i].ypos + ypos - B_HEIGHT) - (SCREEN_HEIGHT / 2) - UNI_HEIGHT <= fallonblock) || fallonblock == 0)
						{
							fallonblock = (b[i].ypos + ypos - B_HEIGHT) - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
							ison = i;
						}
					}
					else//jezeli nie jest nad to powinien upasc na ziemie
					{
						fallonground = g[0].ypos + ypos - B_HEIGHT / 2 - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
					}
				}
				else//jezeli nie pokrywaja sie x to takze zapamietaj wspolrzedne ziemii
				{
					fallonground = g[0].ypos + ypos - B_HEIGHT / 2 - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
				}
			}//platformy
			else
			{
				if ((b[i].xpos + xpos - P_WIDTH) < UNICORN_X + UNI_WIDTH && (b[i].xpos + xpos + P_WIDTH) > UNICORN_X - UNI_WIDTH)
				{
					if (b[i].ypos + ypos + B_HEIGHT / 2 > UNICORN_Y - UNI_HEIGHT && b[i].ypos + ypos - B_HEIGHT / 2 < UNICORN_Y + UNI_HEIGHT)
					{
						collision = true;
					}
					else if (UNICORN_Y + UNI_HEIGHT <= b[i].ypos + ypos - B_HEIGHT / 2)
					{
						if ((b[i].ypos + ypos - B_HEIGHT / 2) - (SCREEN_HEIGHT / 2) - UNI_HEIGHT <= fallonblock || fallonblock == 0)
						{
							fallonblock = (b[i].ypos + ypos - B_HEIGHT / 2) - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
							ison = i;
						}
					}
					else
					{
						fallonground = g[0].ypos + ypos - B_HEIGHT / 2 - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
					}
					//sprawdzenie czy nie jest pod platform¹
					if (UNICORN_Y - UNI_HEIGHT > b[i].ypos + ypos + B_HEIGHT && UNICORN_Y - UNI_HEIGHT <= b[i].ypos + ypos + B_HEIGHT + JUMP)
					{
						canjump = 0;
					}
				}
				else
				{
					fallonground = g[0].ypos + ypos - B_HEIGHT / 2 - (SCREEN_HEIGHT / 2) - UNI_HEIGHT;
				}
			}
		}
	}



	void movecheck(bool mup, bool& falling, int canjump, double& ypos, double& fallingTimer, bool mleft, double& xpos, bool mright, double delta, int ison, int fallonblock, int fallonground)
	{
		if (mup == true && falling == false && canjump == 1)
		{
			ypos = JUMP;
			falling = true;
			fallingTimer = 0;
		}
		else
			ypos = 0;

		if (mleft == true)
			xpos = SPEED;
		else if (mright == true)
			xpos = -SPEED;
		else
			xpos = 0;

		if (falling == true)
		{
			fallingTimer += delta;
			if (fallingTimer > 0.5)
			{
				if (ison >= 0)
					ypos = -fallonblock;
				else if (ison < 0)
					ypos = -fallonground;
				fallingTimer = 0;
				falling = false;
			}
		}
		else if (ison < 0)
			ypos = -fallonground;
		else if (ison >= 0)
			ypos = -fallonblock;
	}



	void newgamecreate(int* t1, int* frames, double* worldTime, double* fpsTimer, double* fps, double* distance, int* newgame, double* xpos, double* ypos, bool* falling, double* fallingTimer, Blocks b[], int* fallonblock, int* fallonground, bool* collision, Blocks g[], int* ison, int* canjump)
	{
		*t1 = SDL_GetTicks();
		*frames = 0;
		*fpsTimer = 0;
		*fps = 0;
		*worldTime = 0;
		*distance = 0;
		*newgame = 0;
		*xpos = 0;
		*ypos = UNICORN_Y - 7;
		*falling = false;
		*fallingTimer = 0;
		int k = 1;
		for (int i = 0, j = BLOCK_FREQUENCY; i < MAX_B; i++, j += BLOCK_FREQUENCY)
		{
			if (i < MAX_B / 2)
			{
				b[i].xpos = j;
				b[i].ypos = *ypos;
			}
			else if (i < MAX_B - 1)
			{
				b[i].xpos = j;
				b[i].ypos = *ypos - k * 100;
				k++;
			}
			else
			{
				b[i].xpos = j - BLOCK_FREQUENCY / 2;
				b[i].ypos = *ypos - k * 100;
			}

		}
		*fallonblock = 0;
		*fallonground = 0;
		*collision = false;
		for (int i = 0; i < MAX_G; i++)
		{
			g[i].xpos = BLOCK_FREQUENCY + (i * SCREEN_WIDTH);
			g[i].ypos = UNICORN_Y + 41;
		}
		*ison = -1;
		*canjump = 1;
	}



	void loadmedia(SDL_Surface*& charset, SDL_Surface*& screen, SDL_Surface*& unicorn, SDL_Texture*& scrtex, SDL_Window*& window, SDL_Renderer*& renderer, SDL_Surface*& background, SDL_Surface*& block, SDL_Surface*& ground, SDL_Surface*& platform)
	{
		bool flag = true;

		charset = SDL_LoadBMP("./cs8x8.bmp");
		if (charset == NULL)
		{
			printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};
		SDL_SetColorKey(charset, true, 0x000000);

		unicorn = SDL_LoadBMP("./unicorn.bmp");
		if (unicorn == NULL)
		{
			printf("SDL_LoadBMP(unicorn.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};

		background = SDL_LoadBMP("./background.bmp");
		if (background == NULL)
		{
			printf("SDL_LoadBMP(background.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};

		block = SDL_LoadBMP("./block.bmp");
		if (block == NULL)
		{
			printf("SDL_LoadBMP(block.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};

		ground = SDL_LoadBMP("./ground.bmp");
		if (ground == NULL)
		{
			printf("SDL_LoadBMP(ground.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};

		platform = SDL_LoadBMP("./platform.bmp");
		if (platform == NULL)
		{
			printf("SDL_LoadBMP(platform.bmp) error: %s\n", SDL_GetError());
			flag = false;
		};

		if (flag == false)
		{
			programend(&screen, &charset, &unicorn, &scrtex, &window, &renderer, &background, &block, &ground, &platform);
			exit(0);
		}
	}



	void InfoText(SDL_Surface* screen, int red, int blue, char  text[128], double worldTime, double fps, SDL_Surface* charset)
	{
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, red, blue);
		sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		sprintf(text, "Esc - wyjscie, N - nowa gra, \030 \032 \033 - sterowanie");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
	}



	bool initialization(SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** screen, SDL_Texture** scrtex)
	{
		int rc;
		bool flag = true;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			printf("SDL_Init error: %s\n", SDL_GetError());
			flag = false;
		}
		else
		{
			// tryb pe³noekranowy
			// rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
			rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);
			if (rc != 0)
			{
				printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
				flag = false;
			}
		}
		if (flag == true)
		{
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
			SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
			SDL_SetWindowTitle(*window, "Szablon do zdania drugiego 2017");

			*screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

			*scrtex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

			// wy³¹czenie widocznoœci kursora myszy
			SDL_ShowCursor(SDL_DISABLE);
		}

		return flag;
	}



	void programend(SDL_Surface** screen, SDL_Surface** charset, SDL_Surface** unicorn, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer, SDL_Surface** background, SDL_Surface** block, SDL_Surface** ground, SDL_Surface** platform)
	{
		SDL_FreeSurface(*screen);
		SDL_FreeSurface(*charset);
		SDL_FreeSurface(*unicorn);
		SDL_FreeSurface(*background);
		SDL_DestroyTexture(*scrtex);
		SDL_DestroyWindow(*window);
		SDL_DestroyRenderer(*renderer);
		SDL_Quit();
		exit(0);
	}



	void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) 
	{
		int px, py, c;
		SDL_Rect s, d;
		s.w = 8;
		s.h = 8;
		d.w = 8;
		d.h = 8;
		while (*text) 
		{
			c = *text & 255;
			px = (c % 16) * 8;
			py = (c / 16) * 8;
			s.x = px;
			s.y = py;
			d.x = x;
			d.y = y;
			SDL_BlitSurface(charset, &s, screen, &d);
			x += 8;
			text++;
		};
	};



	void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) 
	{
		SDL_Rect dest;
		dest.x = x - sprite->w / 2;
		dest.y = y - sprite->h / 2;
		dest.w = sprite->w;
		dest.h = sprite->h;
		SDL_BlitSurface(sprite, NULL, screen, &dest);
	};



	void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) 
	{
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	};



	void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color)
	{
		for (int i = 0; i < l; i++)
		{
			DrawPixel(screen, x, y, color);
			x += dx;
			y += dy;
		};
	};



	void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor)
	{
		int i;
		DrawLine(screen, x, y, k, 0, 1, outlineColor);
		DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
		DrawLine(screen, x, y, l, 1, 0, outlineColor);
		DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
		for (i = y + 1; i < y + k - 1; i++)
			DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};