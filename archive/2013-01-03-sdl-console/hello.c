//** hello.c : CODENAME HELLO : DartPower's Team, NWO 2013
//** LEGAL TO COPY, EDIT, STEAL AND TRADE
#include "SDL_main.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keysim.c"
#include "acsman.c"
//#include "maya.c" //нахуй рубик он не тру, вместо его сделаем календарь Майа

#include <GL/gl.h>
#include <GL/glu.h>

#include "helleventcontrol.c"

#define BUF_SIZE 254
#define CONT_SIZE 512
#define PHR_INITIAL 23
#define UNI_MAX_WIDE_CHAR 4
#define USERINPUT_WELCOME_TEXT "> "
#define USERINPUT_WELCOME 2
		
#define length(x) (sizeof x / sizeof *x)

static GLboolean should_rotate = GL_TRUE;
/*
 * BIG NOTE BIG NOTE
 * MUCH ENDIANNESS LACKS!!!! THE CODE ******MUST****** BE FURTHER **REFORMATED**
 */

int insert(int *array, int val, size_t length, size_t at) //функция небезопасна но буду делать всю безоапаность перед тем как её вызывать
{
  size_t i = length - 1;

  /* сдвигаем, деля дыру */
  while (i > at)
  {
    array[i] = array[i - 1];
    i--;
  }
  /* Вброс! */
  array[at] = val;

  return 0;
}

int detach(int *array, size_t length, size_t at) //обратное от insert - убирает элемент at и сдвигает остальные Небезопасно: не перерасчитывает память
{
  size_t i = at;

  /* сдвигаем, образуется дырка */
  while (i < length - 1)
  {
    array[i] = array[i + 1];
    i++;
  }
  /* удаление нахуй! */
  array[length-1] = 0;
  return 0;
}

size_t input_string_length(char *string,int *actual)
{
  int len=strlen(string);
  int x=1;
  int res=0;
  int alen=0;
  while(*(string+len-x))
  {
    if((unsigned int)*(string+len-x)<=127)
    {
      res+=4;
      alen++;
      x++;
    }
    else
    {
      if(((int)*(string+len-x) | 0x80)==(int)*(string+len-x)) // | 0xC0
      {
	if(((int)*(string+len-x-1) | 0xC0)==(int)*(string+len-x-1))
	{
	  res+=4;
	  alen++;
	  x+=2;
	}
	else if((*(string+len-x-1) | 0x80)==(int)*(string+len-x-1)) // | 0xC0
	{
	  if(((int)*(string+len-x-2) | 0xC0)==(int)*(string+len-x-2)) // | 0xC0
	  {
	    res+=4;
	    alen++;
	    x+=3;
	  }
	  else if(((int)*(string+len-x-2) | 0x80)==(int)*(string+len-x-2)) // | 0xC0
	  {
	    if(((int)*(string+len-x-3) | 0xC0)==(int)*(string+len-x-3)) // | 0xC0
	    {
	      res+=4;
	      alen++;
	      x+=4;
	    }
	    else
	    {
	      res+=4;
	      alen++;
	      x++;
	    }
	  }
	  else
	  {
	    res+=4;
	    alen++;
	    x++;
	  }
	}
	else
	{
	  res+=4;
	  alen++;
	  x++;
	}
      }
      else
      {
	res+=4;
	alen++;
	x++;
      }
    }
  }
  if(actual!=NULL)
    *actual=alen;
  return(res);
  /*else
  {
*/
}

void DrawImage(SDL_Surface* source, SDL_Surface* destination, int x, int y) {
    if (!source || !destination)
        return;

    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    SDL_BlitSurface(source, NULL, destination, &destRect);
}

char **RndPhr;
char RndPhrase[PHR_INITIAL][BUF_SIZE]={"Привет","Не знаю","Да","Нет","Ну-ну...","Может быть","Серьезно?","Когда?","Как?","Где?","O_o","Ээээ...","Да пошел ты!","Да?","Хватит","???","Не надо.","Хорошо","Зачем?","Не понимаю...","В смысле?","Да ты что!","Я думаю..."};

char *replace(char *st, char *orig, char *repl) {
  static char buffer[4096];
  char *ch;
  if (!(ch = strstr(st, orig)))
   return st;
  strncpy(buffer, st, ch-st);
  buffer[ch-st] = 0;
  sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));
  return buffer;
  }

int ii_round(double number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}

int anay(const void *x, const void *y) {
  return (*(int*)x - *(int*)y);
}

void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

float analyze(char *buffer,char *ptr) //< есть эта
{
  unsigned int equal=0,notequal=1;
  while(*buffer)
  {
    while(*ptr)
    {
      if(!strncmp(ptr,buffer,2))
	equal++;
      else
	notequal++;
      *ptr++;
    }
    *buffer++;
  }
  return(((float)equal/(float)notequal));
}

void ii_sdlvideomode (SDL_Surface *screen,int w, int h, int d,int flags)
{ 
    screen = SDL_SetVideoMode(w, h, d, flags);
    if ( !screen || screen == NULL ) {// Если установить разрешение не удалось
        fprintf(stderr, "Невозможно установить разрешение %dx%d@%dbpp: %s\n", w, h, d, SDL_GetError());
        exit(1);
    }
}

void ii_ttfinit (void)
{
    if (TTF_Init() < 0) {
        fprintf(stderr, "Error: Unable to initialize SDL_ttf");
        exit(1);
    }
}

void setup_opengl( int width, int height )
{
    float ratio = (float) width / (float) height;

    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH );

    /* Culling. */
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );
    glEnable( GL_CULL_FACE );

    /* Set the clear color. */
    glClearColor( 0, 0, 0, 0 );

    /* Setup our viewport. */
    glViewport( 0, 0, width, height );

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    /*
     * EXERCISE:
     * Replace this with a call to glFrustum.
     */
    gluPerspective( 60.0, ratio, 1.0, 1024.0 );
}

int main( int argc, char* argv[] )
{
    const SDL_VideoInfo* info = NULL;
	int bpp = 0;
	int flags = 0;

    SDL_Surface* ii_video = NULL;
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        fprintf(stderr, "Не могу инициализировать SDL: %s\n", SDL_GetError());
        exit(1);
    }
    info = SDL_GetVideoInfo( );
    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n",
             SDL_GetError( ) );
        exit(1);
    }
	bpp = info->vfmt->BitsPerPixel;
    /*
     * Now, we want to setup our requested
     * window attributes for our OpenGL window.
     * We want *at least* 5 bits of red, green
     * and blue. We also want at least a 16-bit
     * depth buffer.
     *
     * The last thing we do is request a double
     * buffered window. '1' turns on double
     * buffering, '0' turns it off.
     *
     * Note that we do not use SDL_DOUBLEBUF in
     * the flags to SDL_SetVideoMode. That does
     * not affect the GL attribute state, only
     * the standard 2D blitting setup.
     */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	    /*
     * We want to request that SDL provide us
     * with an OpenGL window, in a fullscreen
     * video mode.
     *
     * EXERCISE:
     * Make starting windowed an option, and
     * handle the resize events properly with
     * glViewport.
     */
    flags = SDL_SWSURFACE; //SDL_OPENGL;// | SDL_FULLSCREEN; SDL_SWSURFACE | 

    ii_ttfinit();
    SDL_EnableUNICODE(1);
    atexit(SDL_Quit);
    atexit(TTF_Quit);
    int w=640,h=480,d=16,tw=0,th=0;
    ii_sdlvideomode(ii_video,w,h,bpp,flags);
//	setup_opengl( w, h );
    ii_video=SDL_GetVideoSurface();
    if (SDL_Flip(ii_video) != 0) { //ура! помогло1
      fprintf(stderr, "Не удалось поменять местами буферы: %s\n", SDL_GetError());
      exit(1);
    }
    SDL_Surface **text_solid; //массивы указателей на указатели на текст
    SDL_Surface **text_userinput;
    text_solid = (SDL_Surface **)malloc( sizeof( SDL_Surface * ) ); //место под указатели над текст в массивах указателей
    size_t text_solid_length=1;
    text_userinput = (SDL_Surface **)malloc( sizeof( SDL_Surface * ) );
    *(text_solid) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
    text_solid[0]=NULL;
    *(text_userinput) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
    text_userinput[0]=NULL;
    SDL_Surface *text_arrowup;
    SDL_Surface *text_arrowdown;
	SDL_Surface *text_form_xplusvel;
	SDL_Surface *text_form_yminsvel;
   
    SDL_Color blue = {0, 255, 80};
    
    SDL_Event event; /* Event structure */
    //проинициализировали первый наш шрифт DejaVuSansMono-Bold 
    //так как он идёт вместе с лицензией, то в отдельной папку, куда мы его положили: "DejaVuSansMono-Bold/"
    //лежат файлы LICENSE (лицензия) и AUTHORS (авторы)
    //так как эта лицензия свободная, мы её принимаем и не нарушаем ничьи авторские права, распростраяя в соответствии с ней
    TTF_Font* ii_DejaVuSansMono_Bold09;
    ii_DejaVuSansMono_Bold09 = TTF_OpenFont("DejaVuSansMono-Bold/DejaVuSansMono-Bold.ttf", 12);
    if (!ii_DejaVuSansMono_Bold09) { //проверка, загрузился ли
      TTF_SetError("Loading failed :( (code: %d)", 142);
      fprintf(stderr, "Error: %s",TTF_GetError()); //у нас fprintf а не std::cout так как язык C а не C++
      exit(1);
    }
    if (TTF_SizeText(ii_DejaVuSansMono_Bold09, "A", &tw, &th) < 0) //используем A для sizetext, потому что знаем что у нас моноширинный шрифт
    {
      fprintf(stderr, "Error: Unable to retrieve size of text");
      exit(1);
    }
    TTF_SetFontStyle(ii_DejaVuSansMono_Bold09, TTF_STYLE_NORMAL);
    int lineSkip = TTF_FontLineSkip(ii_DejaVuSansMono_Bold09);
    int rows = (int) ((float)h / lineSkip);
    
    //так отдохнули, теперь надо будет его испытать, этот шрифт)))
    //задаём рисунок с текстом
 
    /*                                      */
    /* SDL инициализирован, дальше мы сами) */
    /*                                      */
    char **RndPhr;
    char *tmppointer;
    char arrowstring[7]={'\0'};
    char arrowstring2[7]={'\0'};
    char printString[BUF_SIZE]={'\0'};
	char xpvelString[BUF_SIZE]={'\0'};
	char ymvelString[BUF_SIZE]={'\0'};
    char userInputString[BUF_SIZE+USERINPUT_WELCOME]={'\0'}; //USERINPUT_WELCOME - скажем нет волшебным числам
    
    signed int shiftcounter=0;
    unsigned int phr_count=PHR_INITIAL;
    /* Занять память. Мы могли бы просто определить массив, но это было бы неэффективно */
    RndPhr = (char **)malloc( PHR_INITIAL* sizeof( char * ) ); //Указатель на массив указателей на символы
    int j=0,k=0;
    char sgn[BUF_SIZE];

    fprintf(stdout,"111\n");
    while(j < PHR_INITIAL)
    {
      
        *(RndPhr+j) = (char *)malloc( strlen(RndPhrase[j])*sizeof( char ) ); //масивы указателей на символы
	//printf("%s\n",RndPhrase[j]);
	RndPhr[j]=RndPhrase[j];
	j++;
    }
   
    srand(time(NULL));
    int i = rand() %PHR_INITIAL;
    
    strcpy(arrowstring,"[↑]");
    strcpy(arrowstring2,"[↓]");
    strcat(printString,"Здарова, ");
    strcat(printString,RndPhr[i]);
    strcat(printString," ёпта");
    strcat(userInputString,USERINPUT_WELCOME_TEXT);
    text_solid[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, printString, blue);
    text_userinput[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
    text_arrowdown=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, arrowstring2, blue);
    text_arrowup=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, arrowstring, blue); //printString - строка, формируемая вместо форматирования при fprintf'е, так как fprintf больше не используем
    
	int charx = 0;
	int chary = 0;
    int x = 0;
    int y = 0;
    short int running = 1;
    char buffer[BUF_SIZE];
    int changed=1;
    int scrollup=0;
  
    FScriptPositionSTCK *FScriptPosition=FScriptPosition_();
    FScriptPosition_Message(FScriptPosition,ii_DejaVuSansMono_Bold09,&changed,&text_solid_length,text_solid,310,BUF_SIZE,"ВНЕЗАПНО: Доступ есть %i",200);
   
    
    while (running) {
      i=rand()%PHR_INITIAL;
      j=1;
      k=phr_count-PHR_INITIAL;
      while(j<=k)
      {
	if(ii_round(((float)(rand()%j))/((float)j))) i++;
	j++;
      }
      if((rand()%10000)==9999&&phr_count>PHR_INITIAL&&scrollup==0)
      {
	strcpy(printString,RndPhr[i]);
	j=text_solid_length;
	text_solid_length++;
	text_solid = (SDL_Surface **)realloc(text_solid, text_solid_length * sizeof( SDL_Surface * ) );
	*(text_solid+j) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
//	SDL_FreeSurface(text_solid[0]);
	text_solid[j]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, printString, blue);
	changed=1;
	}
	
	  process_events(); //нам надо передать туда &event но другим способом нежели(( вот тут нужны идеи
	  /*
      while(SDL_PollEvent(&event)){ //заебись это круто!
	  //теперь нам предстоит огромная работа... всё это вынести в handleevents
	  //начать надо не с практики.. начать надо с бэкапа - создай резервную копию (когда я скажу) нашего исходника
	  //потом предложи какие-нибудь действия по переносу кода мы их обсудим
	  //ну вот, всё))) давай переносить)))
	  //Пошли давай доделаем свой инпут... А то спиздить не удалось ввиду некачественности попила. Ок, го.
	switch(event.type) { /* события типа, чтобы окошки закрывались там *--/
	  case SDL_QUIT:
	    running = 0;
	    break;
	  case SDL_KEYDOWN:  /* чем не кейлоггер, если в фоне? ну ты понел *--/
	    switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				should_rotate = !should_rotate;
			    if(input_string_length(userInputString,NULL)<(BUF_SIZE+USERINPUT_WELCOME))
			      if ((event.key.keysym.unicode & 0xFF80) == 0)
				append(userInputString,(char)event.key.keysym.unicode & 0x7f);
			      else
				unicsym2char(userInputString,event.key.keysym.unicode);
			    SDL_FreeSurface(text_userinput[0]);
			    text_userinput[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
			    changed=1;
			    break; //сука бля имей ввиду кусок хуя я тя временно так сделал потом ты у меня отдельной переменной валяться будешь ты понял?
 			case SDLK_ESCAPE:
			    running = 0;
			    break;
			case SDLK_PAGEDOWN:
			    if(scrollup>0)
			    {
			      scrollup--;
			      changed=1;
			    }
			    break;
			case SDLK_PAGEUP:
			    if((text_solid_length>(rows-1))&&(scrollup<(text_solid_length-(rows-1))))
			    {
			      scrollup++;
			      changed=1;
			    }
			    break;
			case SDLK_BACKSPACE: // Помнишь, я запилил по спецификации тут!! Это прикольно, так немного где
			    if(strlen(userInputString)>USERINPUT_WELCOME) //тут сложные бинарные сопоставления, чтобы не стирать по половине/трети/четверти UTF-8 символов
			    {
			      if((unsigned int)userInputString[strlen(userInputString)-1]<=127)
				userInputString[strlen(userInputString)-1]='\0';
			      else
			      {
				if(((int)userInputString[strlen(userInputString)-1] | 0x80)==(int)userInputString[strlen(userInputString)-1]) // | 0xC0
				{
				  if(((int)userInputString[strlen(userInputString)-2] | 0xC0)==(int)userInputString[strlen(userInputString)-2]) // | 0xC0
				    userInputString[strlen(userInputString)-2]='\0';
				  else if(((int)userInputString[strlen(userInputString)-2] | 0x80)==(int)userInputString[strlen(userInputString)-2]) // | 0xC0
				  {
				    if(((int)userInputString[strlen(userInputString)-3] | 0xC0)==(int)userInputString[strlen(userInputString)-3]) // | 0xC0
				      userInputString[strlen(userInputString)-3]='\0';
				    else if(((int)userInputString[strlen(userInputString)-3] | 0x80)==(int)userInputString[strlen(userInputString)-3]) // | 0xC0
				    {
				      if(((int)userInputString[strlen(userInputString)-4] | 0xC0)==(int)userInputString[strlen(userInputString)-4]) // | 0xC0
					userInputString[strlen(userInputString)-4]='\0';
				      else
					userInputString[strlen(userInputString)-1]='\0';
				    }
				    else
				      userInputString[strlen(userInputString)-1]='\0';
				  }
				  else
				    userInputString[strlen(userInputString)-1]='\0';
				}
				else
				  userInputString[strlen(userInputString)-1]='\0';
			      }
//				fprintf(stdout,"%i",userInputString[strlen(userInputString)-1]);
			      SDL_FreeSurface(text_userinput[0]);
			      text_userinput[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
			      changed=1;
			      break;
			    }
			case SDLK_RETURN:
			    j=text_solid_length;
			    text_solid_length++;
			    text_solid = (SDL_Surface **)realloc(text_solid, text_solid_length * sizeof( SDL_Surface * ) );
			    *(text_solid+j) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
			    text_solid[j]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
			    if(strlen(userInputString)<(USERINPUT_WELCOME+2))
			    {
			      strcpy(printString,"Иди нахуй, окда?");
			      j=text_solid_length;
			      text_solid_length++;
			      text_solid = (SDL_Surface **)realloc(text_solid, text_solid_length * sizeof( SDL_Surface * ) );
			      *(text_solid+j) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
//			      SDL_FreeSurface(text_solid[0]);
			      text_solid[j]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, printString, blue);
			      j=0;
			      changed=1;
			      while(j < phr_count)
			      {
				fprintf(stdout,"%s\n",RndPhr[j]);
				j++;
			      }
			    }
			    else
			    {
			      strcpy(buffer,userInputString+USERINPUT_WELCOME);
			      j=phr_count;
			      phr_count++;
			      
			      RndPhr = (char **)realloc(RndPhr, phr_count * sizeof( char * ) );
			      *(RndPhr+j) = (char *)malloc( strlen(buffer)*sizeof( char ) );
			      strcpy(RndPhr[j],buffer); //го кодить?
			      i=rand()%PHR_INITIAL;
			      j=1;
			      k=phr_count-PHR_INITIAL;
			      while(j<=k)
			      {
				if(ii_round(((float)(rand()%j))/((float)j))) i++;
				j++;
			      }
			      j=0;
			      while(j<phr_count) //24-0 = 24; 24-1=23 и т.п.
			      { 
				shiftcounter=ii_round(phr_count*analyze(buffer,RndPhr[j]));
				if(shiftcounter>j)//если 2 > 24 (нет), если 2 > 23 (нет), если 12 > 24-13 : если 12 > 11 то да а если 1 > 24-23... нет, ура
				  shiftcounter=j;
				if(shiftcounter) //если надо сдвигать элементы то перемещаем выше ( j - количество сдвигов) сдвигов должно быть не больше чем j
				{
				  k=0;
				  while(k<shiftcounter)
				  {
				    tmppointer=RndPhr[j-k]; //мы работаем с указателями на слова, а не со словами, то есть не занимаем память дважды, трижды... Окда?
				    RndPhr[j-k]=RndPhr[j-k-1];
				    RndPhr[j-k-1]=tmppointer;//shiftcounter = 3 значит пока k <3 выполнять: поменять местами исходный и предыдущий (k=0), предыдущий и ещё предыдущий (k=1), ещё предыдыдущий и ещё ещё предыдыдущий (k=2) j-2 != j-3... FAIL. А если <= ? всё-равно FAIL... Превысит допустимое... при k=2 получится j-k-1 = j -3 = j - shiftcounter значит WIN
				    k++;
				  }
				}
				j++;
			      }
			      strcpy(printString,RndPhr[i]);
			      j=text_solid_length;
			      text_solid_length++;
			      text_solid = (SDL_Surface **)realloc(text_solid, text_solid_length * sizeof( SDL_Surface * ) );
			      *(text_solid+j) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
//			      SDL_FreeSurface(text_solid[0]);
			      text_solid[j]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, printString, blue);
			      changed=1;
			    }
			    strcpy(userInputString,"> ");
			    SDL_FreeSurface(text_userinput[0]);
			    text_userinput[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
			    scrollup=0;
			    break;
			default:
			    if(input_string_length(userInputString,NULL)<(BUF_SIZE+USERINPUT_WELCOME))
			      if ((event.key.keysym.unicode & 0xFF80) == 0)
				append(userInputString,(char)event.key.keysym.unicode & 0x7f);
			      else
				unicsym2char(userInputString,event.key.keysym.unicode);
			    SDL_FreeSurface(text_userinput[0]);
			    text_userinput[0]=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, userInputString, blue);
			    changed=1;
			    break;
	    
	    break;
		}
//	  case SDL_MOUSEMOTION: //мышь сделаю позже
//	    printf("Oh! Mouse moved\n");
//	    break;
	  default: /* нахуй анхандлд - не нужен (c) *--/
	    break;
	   // fprintf(stderr, "I don't know what this event is!\n");
	}

	SDL_FillRect(ii_video, NULL, SDL_MapRGB(ii_video->format, 0, 0, 0)); //пизда рулю, этоу функции нужна работающая двойная буферизация!!1адынадыне
	//int hiddenrows = text_solid_length - (rows-1) ;
	//xpvelString[0]='\0';
	//ymvelString[0]='\0';
	
	//sprintf(xpvelString, "%i %i", charx, charxvel);
	//sprintf(ymvelString, "%i %i", chary, charyvel);
	
	//text_form_xplusvel=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, xpvelString, blue);
	//text_form_yminsvel=TTF_RenderUTF8_Blended(ii_DejaVuSansMono_Bold09, ymvelString, blue);

	//input_string_length(xpvelString,&k);
	//DrawImage(text_form_xplusvel, ii_video, w-k*tw, 1*th);
	//input_string_length(ymvelString,&k);
	//DrawImage(text_form_yminsvel, ii_video, w-k*tw, 2*th);
	if((text_solid_length>(rows-1))&&(scrollup<(text_solid_length-(rows-1))))
	{
	  input_string_length(arrowstring,&k);
	  DrawImage(text_arrowup, ii_video, w-k*tw, 0);
	}
	if(scrollup>0)
	{
	  input_string_length(arrowstring2,&k);
	  DrawImage(text_arrowdown, ii_video, w-k*tw, h-lineSkip);
	}
	DrawImage(text_userinput[0], ii_video, x, h-lineSkip);
	j=scrollup;
	while((j-scrollup)<(rows-1)&&j<text_solid_length) //если увеличить разрешение до 1024x768 то глючит зато 320x240 и 640x480 работают ща думаю как поправить высокие: rows = 42 j = text_solid_length - scrollup  text_solid_length > 41
	{
	  DrawImage(text_solid[text_solid_length-1-j], ii_video, x, h-(((j-scrollup)+2)*lineSkip)); //тут всё ОК
	  j++;
	}
	SDL_Flip(ii_video);
	changed=0;
      }
	  */
      SDL_Delay(1);
    }
    j=0;
    while(j<length(text_solid))
      SDL_FreeSurface(text_solid[j++]);
    j=0;
    while(j<length(text_userinput))
      SDL_FreeSurface(text_userinput[j++]);
    TTF_CloseFont(ii_DejaVuSansMono_Bold09);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
