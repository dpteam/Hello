#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h" //фх єърч√трхЄё  ўЄю яЁюхъЄ ╤ р эх c++?

#define BUF_SIZE 90
#define CONT_SIZE 512
#define PHR_INITIAL 23

#ifdef __WIN32__
#include <conio.h>
#include <windows.h>
#include <winioctl.h>

void get_window_size( int *lines, int *columns )
{
CONSOLE_SCREEN_BUFFER_INFO csbi;
if (GetConsoleScreenBufferInfo(
      GetStdHandle( STD_OUTPUT_HANDLE ),
      &csbi
      ))
  {
  lines   = csbi.srWindow.Bottom -csbi.srWindow.Top  +1;
  columns = csbi.srWindow.Right  -csbi.srWindow.Left +1;
  }
else lines = columns = 0;
}

void set_window_size( int lines, int columns )
{
CONSOLE_SCREEN_BUFFER_INFO csbi;
if (GetConsoleScreenBufferInfo(
      GetStdHandle( STD_OUTPUT_HANDLE ),
      &csbi
      ))
  {
  // Убедимся что новый размер не слишком велик
  if (lines   > csbi.dwSize.Y) lines   = csbi.dwSize.Y;
  if (columns > csbi.dwSize.X) columns = csbi.dwSize.X;

  // Если надо, подстроить отображение окна
  if ((csbi.srWindow.Top  +lines)   > csbi.dwSize.Y) csbi.srWindow.Top  = csbi.dwSize.Y -lines   -1;
  if ((csbi.srWindow.Left +columns) > csbi.dwSize.Y) csbi.srWindow.Left = csbi.dwSize.X -columns -1;

  // Вычислить новый размер
  csbi.srWindow.Bottom = csbi.srWindow.Top  +lines   -1;
  csbi.srWindow.Right  = csbi.srWindow.Left +columns -1;

  SetConsoleWindowInfo(
    GetStdHandle( STD_OUTPUT_HANDLE ), 1,&csbi.srWindow );
  }
}
#else
/* termios */
/* мне под линуксом это очень удобно, а под виндой есть свой аналог */
#include <sys/ioctl.h>
#include "termiosdef.c"
 
void get_window_size( int *lines, int *columns )
{
    struct winsize max;
    ioctl(0, TIOCGWINSZ , &max);
    *lines=max.ws_row;
    *columns=max.ws_col;
}
#endif

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

ii_sdlvideomode (SDL_Surface *screen)
{ 
    screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);
    if ( screen == NULL ) {// Если установить разрешение не удалось
        fprintf(stderr, "Невозможно установить разрешение 640x480: %s\n", SDL_GetError());
        exit(1);
    }
    return(screen);
}

int main(void)
{
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Не могу инициализировать SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    SDL_Surface *ii_video;
    ii_sdlvideomode(ii_video);
    char **RndPhr;
    int *tmppointer;
    signed int shiftcounter=0;
    unsigned int phr_count=PHR_INITIAL;
    /* Занять память. Мы могли бы просто определить массив, но это было бы неэффективно */
    RndPhr = (char **)malloc( PHR_INITIAL* sizeof( char * ) ); //Указатель на массив указателей на символы
    int j=0,k=0;
    char sgn[BUF_SIZE];
    
    /* это делает массив практически динамическим */
    while(j < PHR_INITIAL)
    {
      
        *(RndPhr+j) = (char *)malloc( strlen(RndPhrase[j])*sizeof( char ) ); //масивы указателей на символы
	//printf("%s\n",RndPhrase[j]);
	RndPhr[j]=RndPhrase[j];
	j++;
    }
   
    srand(time(NULL));
    int i = rand() %23;
    
    char buffer[BUF_SIZE];
    printf("Здарова, %s ёпт\n\r",RndPhr[i]);
    while(1)
    {
      if((j=getch()))
      {
	if(j!='\n'&&j!='\r')
	{
	  printf("%c",j);
	  append(buffer,j);
	  continue;
	}
	else
	{
	  printf("%c",j);
	}
      }
      else
      {
	if((rand()%100)==99)
	{
	}
	continue;
      }

	while(strstr(buffer,"\n\n"))
	  strcpy(buffer,replace(buffer,"\n\n","\n"));
	while(strstr(buffer,"\r\r"))
	  strcpy(buffer,replace(buffer,"\r\r","\r"));
	while(strstr(buffer,"\n\r\n\r"))
	  strcpy(buffer,replace(buffer,"\n\r\n\r","\n\r"));
	while(strstr(buffer,"\r\n\r\n"))
	  strcpy(buffer,replace(buffer,"\r\n\r\n","\n\r"));
	while(strstr(buffer,"\n\r"))
	 strcpy(buffer,replace(buffer,"\n\r","\0"));
	while(strstr(buffer,"\n"))
	 strcpy(buffer,replace(buffer,"\n","\0"));
	while(strstr(buffer,"\r"))
	 strcpy(buffer,replace(buffer,"\r","\0"));

	if(strlen(buffer)<2)
	{
	   perror("Иди нахуй, окда?");
	    j=0;
	    while(j < phr_count)
	    {
	      //printf("%s\n",RndPhr[j]);
	      j++;
	    }
	   
	   exit(4);
	}
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
	
	
	
	printf("%s\n\r",RndPhr[i]); //%s,%d,%s
	j=0;
	while(j<phr_count) { //24-0 = 24; 24-1=23 и т.п.
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
	buffer[0]='\0';
    }
    
    return 0;
}
