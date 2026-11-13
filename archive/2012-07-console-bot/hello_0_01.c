#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 90
#define CONT_SIZE 512
#define PHR_INITIAL 23

char **RndPhr;
char RndPhrase[PHR_INITIAL][BUF_SIZE]={"Привет","Не знаю","Да","Нет","Ну-ну...","Может быть","Серьезно?","Когда?","Как?","Где?","O_o","Ээээ...","Да пошел ты!","Да?","Хватит","???","Не надо.","Хорошо","Зачем?","Не понимаю...","В смысле?","Да ты что!","Я думаю..."};

//char FILE_HADER[] = {0x55, 0x5f, 0x53, 0x41, 0x56, 0x45, 0x0, 0xff};

int round(const float a)
{
  return((int)(a+0.5));
}

int anay(const void *x, const void *y) {
  return (*(int*)x - *(int*)y);
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

int main(void)
{
    int **RndPhr;
    int *tmppointer;
    signed int shiftcounter=0;
    unsigned int phr_count=PHR_INITIAL;
    RndPhr = (int **)malloc( PHR_INITIAL* sizeof( char * ) ); //Указатель на массив указателей на символы
    int j=0,k=0;
    char sgn[BUF_SIZE];
    
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
    char *newwrd;
    newwrd = (char *)malloc( BUF_SIZE * sizeof( char ) );
    size_t contentSize = 1; // includes NULL
    /* Preallocate space.  We could just allocate one char here, 
    but that wouldn't be efficient. */
    char *content = malloc(sizeof(char) * CONT_SIZE);

    
//malloc(sizeof(char) * BUF_SIZE * PHR_INITIAL);

    //exit(0);
    //phr_word *RndPhrase;
    //RndPhrase = &RndPhr;
    printf("Здарова, %s ёпт\n",RndPhr[i]); //%s,%d,%s
    if(content == NULL)
    {
	perror("Нихуя себе");
	exit(1);
    }
    content[0] = '\0'; // make null-terminated
    newwrd[0] = '\0';
    while(fgets(buffer, BUF_SIZE, stdin))
    {
	char *old = content;
	newwrd = '\0';
	contentSize += strlen(buffer);
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
	content = realloc(content, contentSize);
	if(content == NULL)
	{
	    perror("Пацан, это репчик");
	    free(old);
	    exit(2);
	}
	j=phr_count;
	phr_count++;
	
	RndPhr = (int **)realloc(RndPhr, phr_count * sizeof( char * ) );
	*(RndPhr+j) = (char *)malloc( strlen(buffer)*sizeof( char ) );
	//RndPhrase=realloc(RndPhrase,sizeof(char) * BUF_SIZE * phr_count);
//	strcat(newwrd,buffer);
//	strcat(content, buffer);
	strcpy(RndPhr[j],buffer); //го кодить?
//	printf("1111");
        srand(time(NULL));
        i = rand() %phr_count;
//	printf("%d ;%d ; %s ; %s",phr_count,j,RndPhr[j-1],RndPhr[j]);
	printf("%s: %d %s \n",RndPhr[i],(int)strlen(buffer),content); //%s,%d,%s
	j=0;
	while(j<phr_count) { //24-0 = 24; 24-1=23 и т.п.
	  shiftcounter=round(phr_count*analyze(buffer,RndPhr[j]));
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
//	  printf("%s",tmppointer); //короче не тупить паша это указатель и точка
	  
	  //  ; //надо бы сортировать массив ответов так чтобы позиция увеличивалась от совпадения
	  j++;
	}
    }
    
    if(ferror(stdin))
    {
	free(content);
	perror("Заебись блеать");
	exit(3);
    }
    return 0;
 //   getch();
}