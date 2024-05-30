#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 928;
const int SCREEN_HEIGHT = 793;
int hEalth=3 ;


//LoadTexture
class LTexture
{
	public:
		LTexture();
		~LTexture();

		bool loadFromFile( std::string path );

		bool load_FromrenderText(std::string textureText, SDL_Color textColor);

		void free();

		void render( int x, int y, SDL_Rect* clip=NULL, double angle=0.0, SDL_Point* center=NULL, SDL_RendererFlip flip=SDL_FLIP_NONE);

		int getWidth();
		int getHeight();

	private:
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

//anh
LTexture gBackgroundTexture;
LTexture chuongngai1;
LTexture chuongngai2;
LTexture gHealth;
LTexture gMenu;
LTexture howtoplay;
LTexture gOver;

const int ANIMATION_FRAME=3;
int frame = 0;
LTexture gCharacter;
SDL_Rect CharacClip[ANIMATION_FRAME];
SDL_Rect *currentClip;


//thoi gian
LTexture gTime;
LTexture gHighTimeSurvice;
TTF_Font* gFont=NULL;
//am nhac
Mix_Music *menu;
Mix_Music *play;
Mix_Music *over;
Mix_Chunk *Collider;
Mix_Chunk *start;
Mix_Chunk *vacham;

bool init();

bool loadMedia();

bool checkCollision( SDL_Rect a, SDL_Rect b);

void close();

//Cac ham
int showMenu(); //show menu
int gameOver(); //show game over
void saveHighTime(int HIGHTIME); //luu diem cao
int loadHighTime();
//

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	free();
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		cout << "Khong the tai hinh anh "<< endl;
	}
	else
	{
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			cout << "Khong the tao texture từ file "<< endl;
		}
		else
		{
			// Lấy kích thước của hình ảnh
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Loại bỏ bề mặt đã tải cũ
		SDL_FreeSurface( loadedSurface );
	}

	//Tra ve ket qua
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::load_FromrenderText(std::string textureText, SDL_Color textColor)
{
   free();

   SDL_Surface* textSurface=TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
   if(textSurface!=NULL)
   {
      mTexture=SDL_CreateTextureFromSurface(gRenderer, textSurface);
      if(mTexture==NULL)
      {
         cout << "Khong the tao texture tu surface!" << endl;
      }
      else
      {
         mWidth=textSurface->w;
         mHeight=textSurface->h;
      }

      SDL_FreeSurface(textSurface);
   }
   else
   {
      cout << "Khong the tao surface cho van ban" << endl;
   }

   return mTexture!=NULL;
}


void LTexture::free()
{
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if(clip!=NULL)
   {
      renderQuad.w=clip->w;
      renderQuad.h=clip->h;
   }

   SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);

}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

class Character
{
public:
   static const int CHARACTER_WIDTH=45;
   static const int CHARACTER_HEIGHT=45;

   const double CHARACTER_VEL=0.6;

   Character();

   void handleEvent( SDL_Event& e);

   void move();

   void render();

   SDL_Rect getmCollider()
   {
      return mCollider;
   }

   void resetPosition()
   {
      mPosX=500;
      mPosY=680;
      mCollider.x=mPosX;
      mCollider.y=mPosY;
      Vjump=-1.5;
      jump=false;
      up_presses=false;
   }

   double setVelCharac()
   {
      mVelX=0;
      mVelY=0;
      Vjump=0;
   }

private:
   SDL_Rect mCollider;

   double mPosX,mPosY;

   double mVelX,mVelY,Vjump;

   bool jump,up_presses;


};

Character::Character()
{
   mPosX=500;
   mPosY=680;

   jump=false;
   up_presses=false;

   mCollider.w=CHARACTER_WIDTH;
   mCollider.h=CHARACTER_HEIGHT;

   mVelX=0;
   mVelY=0;
   Vjump=-1.5;
}
SDL_RendererFlip flipType=SDL_FLIP_NONE;
void Character::handleEvent(SDL_Event& e)
{
   if(e.type==SDL_KEYDOWN && e.key.repeat==0)
   {
      switch(e.key.keysym.sym)
      {
         case SDLK_RIGHT:
         mVelX+=CHARACTER_VEL;
         flipType=SDL_FLIP_NONE;
         break;
         case SDLK_LEFT:
         mVelX-=CHARACTER_VEL;
         flipType=SDL_FLIP_HORIZONTAL;
         break;
         case SDLK_SPACE:
            if(!jump)
            {
               jump=true;
            }
            up_presses=true;
         break;
      }
   }
   else if(e.type==SDL_KEYUP && e.key.repeat==0)
   {
      switch( e.key.keysym.sym)
      {
         case SDLK_LEFT: mVelX+=CHARACTER_VEL;
         flipType=SDL_FLIP_HORIZONTAL;
         break;
         case SDLK_RIGHT: mVelX-=CHARACTER_VEL;
         flipType=SDL_FLIP_NONE;
         break;
         case SDLK_SPACE:
         up_presses=false;
         break;
      }
   }

}

void Character::move()
{
   if(up_presses && !jump)
   {
      mVelY=Vjump;
      jump=true;
   }
   if(jump)
   {
      mVelY+=0.01;
   }
   mPosY+=mVelY;
   if(mPosY>=680)
   {
      mPosY=680;
      mVelY=0;
      jump=false;
   }


   mPosX+=mVelX;
   mCollider.x=mPosX;
   if((mPosX<0) || ( mPosX+ CHARACTER_WIDTH>SCREEN_WIDTH))
   {
      mPosX-=mVelX;
      mCollider.x=mPosX;
   }
   mCollider.y=mPosY;
}

void Character::render()
{
   currentClip = &CharacClip[frame/4];
   if( mVelX == 0)
   {
      gCharacter.render( mPosX, mPosY, &CharacClip[0], NULL, NULL, flipType);
   }
   else
   {
      gCharacter.render(mPosX, mPosY, currentClip, NULL, NULL, flipType);
      ++frame;
   }
   if( frame/4 >= ANIMATION_FRAME)
   {
      frame=0;
   }
}


int random(int minN,int maxN)
{
   return minN+rand()%(maxN+1-minN);
}

class ThreatsObject
{
public:
   static const int Threats_WITDH=100;
   static const int Threats_HEIGHT=100;

   ThreatsObject();

   void move(Character& character);

   void render();

   SDL_Rect getmCollider()
   {
      return mCollider;
   }

   void resetPosition()
   {
      mPosX=random(0,8)*100;
      mPosY=random(0,3)*-300;
      mCollider.x=mPosX;
      mCollider.y=mPosY;
      mVelY=0.5;
   }

   double setVelY()
   {
      mVelY=0;
   }
   void setVelY2(double y1){mVelY += y1;}



private:
   SDL_Rect mCollider;

   double mPosX,mPosY;

   double mVelX,mVelY;



};

ThreatsObject::ThreatsObject()
{
   mPosX=random(0,8)*100;
   mPosY=random(1,3)*-300;

   mCollider.w=Threats_WITDH;
   mCollider.h=Threats_HEIGHT;

   mVelY=0.5;
   mVelX=0;
}

void ThreatsObject::move( Character& character)
{
   mPosY+=mVelY;
   mCollider.y=mPosY;
   mCollider.x=mPosX;


   if(mPosY+Threats_HEIGHT> SCREEN_HEIGHT - 40  || checkCollision(getmCollider(),character.getmCollider()) )
   {
      Mix_PlayChannel(-1,Collider,0);
      mPosY=-15-Threats_HEIGHT;
      mPosX=random(0,8)*100;
   }
}

void ThreatsObject::render()
{
   chuongngai1.render(mPosX,mPosY);
}

class TenLua
{
public:
   static const int TenLua_WITDH=100;
   static const int TenLua_HEIGHT=73;

   TenLua();

   void move(Character& character);

   void render();

   SDL_Rect getmCollider()
   {
      return mCollider;
   }

   void resetPosition()
   {
      mPosX=0;
      mPosY=665;
      mCollider.x=mPosX;
      mCollider.y=mPosY;
      mVelX=0.15;
   }

   double setVelX()
   {
      mVelX=0;
   }
private:
   SDL_Rect mCollider;

   double mPosX,mPosY;

   double mVelX,mVelY;

};

TenLua::TenLua()
{
   mPosX=0;
   mPosY=665;

   mCollider.w=TenLua_WITDH;
   mCollider.h=TenLua_HEIGHT;

   mVelY=0;
   mVelX=0.15;
}

void TenLua::move(Character& character)
{
   mPosX+=mVelX;

   mCollider.x=mPosX;
   mCollider.y=mPosY;

   if(mPosX+TenLua_WITDH>SCREEN_WIDTH+10 || checkCollision(getmCollider(),character.getmCollider()))
   {
      Mix_PlayChannel(-1,Collider,0);
      mPosX=-500;
   }
}

void TenLua::render()
{
   chuongngai2.render(mPosX,mPosY);
}

class Health
{
public:
   static const int ICON_WITDH=50;
   static const int ICON_HEIGHT=40;
   Health();

   void render(int X);
private:
   int mPosX,mPosY;

};

Health::Health()
{
   mPosY=10;
}

void Health::render(int X)
{
   gHealth.render(X,10);
}

bool init()
{
	bool success = true;

	//Khoi tao SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		cout<<"Khong the khoi tao SDL"<<endl;
		success = false;
	}
	else
	{
		//Tao cua so
		gWindow = SDL_CreateWindow( "Run And Dodge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			cout<<"Khong the tao cua so"<<endl;
			success = false;
		}
		else
		{
			//Tao renderer cho cua so
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
            cout<<"Khong the tao renderer"<<endl;
				success = false;
			}
			else
			{
				//Khoi tao mau cho renderer
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Khoi tao anh PNG
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					cout<<"Khong the khoi tao SDL_image"<<endl;
					success = false;
				}

				//Khoi tao am thanh
				if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0)
            {
               success=false;
            }

            //Khoi tao thu vien TTF
            if(TTF_Init()==-1)
            {
               cout<<"Khong the khoi tao TFF"<<endl;
               success=false;
            }
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	//
	if(!howtoplay.loadFromFile("image/howtoplay.png"))
   {
      cout << "Khong the tai hinh anh howtoplay.png!" << endl;
      success=false;
   }

	if(!gOver.loadFromFile("image/OVER.png"))
   {
      cout << "Khong the tai hinh anh OVER.png!" << endl;
      success=false;
   }

	if(!gMenu.loadFromFile("image/menu.png"))
   {
      cout << "Khong the tai hinh anh menu.png!" << endl;
      success=false;
   }

	if(!gHealth.loadFromFile("image/health.png"))
   {
      cout << "Khong the tai hinh anh health.png!" << endl;
      success=false;
   }

	if(!chuongngai2.loadFromFile("image/tenlua.png"))
   {
      cout << "Khong the tai hinh anh tenlua.png!" << endl;
      success=false;
   }

	if(!chuongngai1.loadFromFile("image/chuongngai.png"))
   {
      cout << "Khong the tai hinh anh chuongngai.png!" << endl;
      success=false;
   }

	if(!gCharacter.loadFromFile("image/Player.png"))
   {
      cout << "Khong the tai hinh anh Player.png!" << endl;
      success=false;
   }
   else
   {
      //set characClip
      CharacClip[0].x = 0;
      CharacClip[0].y = 0;
      CharacClip[0].w = 45;
      CharacClip[0].h = 45;

      CharacClip[1].x = 45;
      CharacClip[1].y = 0;
      CharacClip[1].w = 45;
      CharacClip[1].h = 45;

      CharacClip[2].x = 90;
      CharacClip[2].y = 0;
      CharacClip[2].w = 45;
      CharacClip[2].h = 45;

   }


	if( !gBackgroundTexture.loadFromFile( "image/Background.png" ) )
	{
		cout << "Khong the tai hinh anh Background.png!" << endl;
		success = false;
	}
	//

	//
	menu=Mix_LoadMUS("sound/menu.mp3");
	if(menu==NULL)
   {
      success=false;
   }

   play=Mix_LoadMUS("sound/play.mp3");
	if(play==NULL)
   {
      success=false;
   }

   over=Mix_LoadMUS("sound/over.mp3");
	if(menu==NULL)
   {
      success=false;
   }

	Collider=Mix_LoadWAV("sound/Collider.wav");
	if(Collider==NULL)
   {
      success=false;
   }
   start=Mix_LoadWAV("sound/start.wav");
	if(start==NULL)
   {
      success=false;
   }
   vacham=Mix_LoadWAV("sound/huh.wav");
	if(vacham==NULL)
   {
      success=false;
   }
   //

   gFont=TTF_OpenFont("front.ttf", 20);
   if(gFont==NULL)
   {
      success=false;
   }

   return success;
}

void close()
{
   //GIAI PHONG HINH ANH
	gCharacter.free();
	gBackgroundTexture.free();

	chuongngai1.free();
	chuongngai2.free();

	gHealth.free();
	gMenu.free();
	gOver.free();
	howtoplay.free();

	//GIAI PHONG NHAC
	Mix_FreeMusic(menu);
	menu=NULL;
	Mix_FreeMusic(play);
	play=NULL;
	Mix_FreeMusic(over);
	over=NULL;
	Mix_FreeChunk(Collider);
	Collider=NULL;
	Mix_FreeChunk(start);
	start=NULL;
	Mix_FreeChunk(vacham);
	vacham=NULL;
	//

	//GIAI PHONG TEXT
	gTime.free();
	gHighTimeSurvice.free();
	TTF_CloseFont(gFont);
	gFont=NULL;
	//


	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
   SDL_bool x=SDL_HasIntersection( &a, &b);
   return x;
}

int showMenu()
{
   bool menuRun=true;
   bool howtoPlay=false;
   bool quit=false;
   Mix_PlayMusic(menu,-1);

   while(menuRun)
   {
      SDL_Event m_event;
      while(SDL_PollEvent(&m_event))
      {
         switch(m_event.type)
         {
         case SDL_QUIT:
            return 2;
            break;
         case SDL_MOUSEBUTTONDOWN:
            if(m_event.button.button==SDL_BUTTON_LEFT)
            {
               int xm=m_event.button.x;
               int ym=m_event.button.y;
               cout<<xm<<" "<<ym<<endl;
               if(xm>327 && xm<610 && ym>328 && ym<474)
               {
                  Mix_PlayChannel(-1,start,0);
                  howtoPlay=true;
                  howtoplay.render(0,0);
               }
            }
            break;
         case SDL_KEYDOWN:
            if(m_event.key.keysym.sym==SDLK_UP && m_event.key.repeat==0)
            {
               return 1;
            }
         }
      }
      if(!howtoPlay)
      {
         gMenu.render(0,0);
      }
      SDL_RenderPresent(gRenderer);
   }
   return 0;

}

int gameOver()
{
   bool gameOverRunning=true;
   Mix_PlayMusic(over,-1);
   while(gameOverRunning)
   {
      SDL_Event m_event;
      while(SDL_PollEvent(&m_event))
      {
         switch (m_event.type)
         {
         case SDL_QUIT:
            return 2;
            break;
         case SDL_MOUSEBUTTONDOWN:
            if(m_event.button.button==SDL_BUTTON_LEFT)
            {
               int xm=m_event.button.x;
               int ym=m_event.button.y;
               cout<<xm<<" "<<ym<<endl;
               //choi lai
               if(xm>256 && xm<388 && ym>440 && ym<501)
               {
                  Mix_HaltMusic();
                  return 1;
               }
               //khong choi lai
               if(xm>548 && xm<667 && ym>440 && ym<501)
               {
                  Mix_HaltMusic();
                  return 2;
               }

            }
         }
      }
      gOver.render(0,0);
      SDL_RenderPresent(gRenderer);
   }
   return 0;
}

void saveHighTime(int HIGHTIME)
{
   ofstream file("hightime.txt");
   if(file.is_open())
   {
      file<<HIGHTIME;
      file.close();
   }
}

int loadHighTime()
{
   int hightime=0;
   ifstream file("hightime.txt");
   if(file.is_open())
   {
      file>>hightime;
      file.close();
   }
   return hightime;
}

int TANGTOC = 1;

int main( int argc, char* args[] )
{
	if( !init() )
	{
		cout<<"Khoi tao that bai"<<endl;
	}
	else
	{
		if( !loadMedia() )
		{
			cout<<"Tai media that bai"<<endl;
		}
		else
		{
			bool quit = true;
			bool menu=true;

			SDL_Event e;

			Character charac;
			TenLua chuongngai2;
			vector <Health> health;
			for(int i=0;i<hEalth;i++)
         {
            health.push_back(Health());
         }

			vector <ThreatsObject> threads;
         for(int i=0;i<4;i++)
         {
            threads.push_back(ThreatsObject());
         }

         //MAU CUA TEXT mau vang
         SDL_Color textColor={225,225,0,255};
         //
         //Thoi gian
         Uint32 startTime;
         int HIGHTIME=loadHighTime();
         stringstream timetext;
         stringstream hightime;

         while(menu)
         {
            int MENU=showMenu();
            if(MENU==1)
            {
               startTime=SDL_GetTicks();
               quit=false;
               menu=false;
            }else if(MENU==2)
            {
               quit=true;
               menu=false;
            }
         }

         Mix_PlayMusic(play,-1);
         //CHAY NHAC NEN CUA GAME
	      int FPS = 60;
	      Uint32 elapsed = 0;
	      int frameTime = 0;
			while( !quit )
			{
				elapsed = SDL_GetTicks();
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					charac.handleEvent( e );
				}

				//HIGHTIME VA TIME
				timetext.str("");
				timetext<<"Time survice : "<<(SDL_GetTicks()-startTime)/1000 <<" s";
				if( !gTime.load_FromrenderText( timetext.str().c_str(), textColor ) )
				{
					cout<<"Khong the hien thi texture thoi gian"<<endl;
					quit=true;
				}

				hightime.str("");
				hightime<<"HighTimeSurvice : "<<HIGHTIME<<" s";
				if(!gHighTimeSurvice.load_FromrenderText(hightime.str().c_str(),textColor))
				{
				   cout<<"Khong the hien thi texture thoi gian"<<endl;
				   quit=true;
				}
				/////

				//TANG VAN TOC CHO VAT THE ROI
				if(((SDL_GetTicks()-startTime)/1000) % 20 == 0 && ((SDL_GetTicks()-startTime)/1000) != 0 && (SDL_GetTicks()-startTime)/1000 / 20 == TANGTOC)
            {
               cout << "tang van toc" << endl;
               TANGTOC++;
               for(auto& x:threads)
               {
                  x.setVelY2(0.2);
               }
            }
            //

				SDL_RenderClear( gRenderer );

				gBackgroundTexture.render( 0, 0);

				gTime.render(SCREEN_WIDTH-300,50);
				gHighTimeSurvice.render(SCREEN_WIDTH-300,20);

				int X=10;
				for(int i=0;i<health.size();i++)
            {
               health[i].render(X);
               X+=50;
            }

				//cap nhat trang thai tro choi
				charac.move();
				charac.render();
				chuongngai2.move(charac);
				chuongngai2.render();

            for(auto& x:threads)
            {
                x.move(charac);
                x.render();
                if(checkCollision(charac.getmCollider(),x.getmCollider()))
                {
                   //Mix_PlayChannel(-1,Collider,0);
                   Mix_PlayChannel(-1,vacham,0);
                   cout<<"va cham"<<endl;
                   hEalth--;
                   health.pop_back();
                }

            }
            if(checkCollision(charac.getmCollider(),chuongngai2.getmCollider()))
            {
               //Mix_PlayChannel(-1,Collider,0);
               Mix_PlayChannel(-1,vacham,0);
               cout<<"va cham"<<endl;
               hEalth--;
               health.pop_back();
            }

            //KET THUC GAME
            if(hEalth<=0)
            {
               int timeOUT=(SDL_GetTicks()-startTime)/1000;
               Mix_PauseMusic();
               for(auto& x:threads)
               {
                  x.setVelY();
               }
               chuongngai2.setVelX();
               charac.setVelCharac();

               //NGUOI CHOI DUA RA Y KIEN
               int playAgain=gameOver();
               //CHOI LAI VA CAP NHAT LAI TRANG THAI, CAP NHAT HIGHTIME
               if(playAgain==1)
               {
                  Mix_PlayMusic(play,-1);
                  cout<<"Choi lai"<<endl;
                  hEalth=3;
                  TANGTOC=1;
                  if(HIGHTIME<=timeOUT)
                  {
                     HIGHTIME=timeOUT;
                  }
                  startTime=SDL_GetTicks();

                  health.clear();
                  for(int i=0;i<hEalth;i++)
                  {
                     health.push_back(Health());
                  }
                  charac.resetPosition();
                  chuongngai2.resetPosition();
                  for(auto& x:threads)
                  {
                     x.resetPosition();
                  }
               }else if(playAgain==2) //NGUOI CHOI KHONG CHOI LAI
               {
                  cout<<"Khong choi lai"<<endl;
                  quit=true;
               }
		         frameTime = SDL_GetTicks() - elapsed;
		         if (1000 / FPS > frameTime){
                     SDL_Delay((1000/FPS)- frameTime);
		         }
            }

				SDL_RenderPresent( gRenderer );

			}
			//LUU THOI GIAN
			saveHighTime(HIGHTIME);
		}
	}


	close();

	return 0;
}
