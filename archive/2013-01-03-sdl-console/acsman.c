//** acsman.c : CODENAME HELLO : DartPower's Team, NWO 2012
//TOTAL REWRITE OF (BY MCCCXXXVII):
//**************************************************************************
//**
//** sc_man.c : Heretic 2 : Raven Software, Corp.
//**
//** $RCSfile: sc_man.c,v $
//** $Revision: 1.3 $
//** $Date: 96/01/06 03:23:43 $
//** $Author: bgokey $
//**
//**************************************************************************

#define MAX_STRING_SIZE 128
#define COBJ_INTSIZE 3
#define TEXTCOLOR_YELLOW {255, 255, 80}
#define TEXTCOLOR_RED {170, 20, 0}
#define TEXTCOLOR_GREEN {0, 255, 20}

typedef struct SavedPos
{
  const char *SavedScriptPtr;
  int SavedScriptLine;
} SavedPos;

//Сука это лучше чем у ренди будь иначе - якорь мне в жопу!
/*
    FString *Test=FString_c("ПРОВЕРКА");
    fprintf(stdout,"%s\n",Test->s);
    FString_v(Test,"ПРОХУЙКА");
    fprintf(stdout,"%s\n",Test->s);
    char testc[MAX_STRING_SIZE]={'\0'};
    FString_tc(testc,Test);
    fprintf(stdout,"%s\n",testc);
    FString *Test=FString_c("ПРОВЕРКА");
    fprintf(stdout,"%s\n",Test->s);
    FString_rp(Test,"ВЕР","ХУЙ"); //йееееес
    FString_rp(Test,"ХУЙ","ХУЙ_ПИЗДА_ДЖИГУРДА"); //йееееес
    fprintf(stdout,"%s\n",Test->s);
    Это ахуенно */

typedef struct FString
{
  char *s;
} FString;

FString *FString_c (const char *s)
{
  FString *a=(FString *) malloc(sizeof(char *));
  a->s=(char *) malloc((strlen(s)+1)*sizeof(char));
  strcpy(a->s,s);
  return(a);
}

FString *FString_cn (const char *s,int len)
{
  FString *a=(FString *) malloc(sizeof(char *));
  a->s=(char *) malloc(len+1);
  strncpy(a->s,s,len);
  a->s[len]=0;
  return(a);
}

void FString_v (FString *old,const char *s)
{
  old->s=(char *) realloc(old->s,(strlen(s)+1)*sizeof(char));
  strcpy(old->s,s);
}

void FString_tc (char *s,FString *a)
{
  int len=0; //понял, надо реаллок сделать
  if(strlen(a->s)>=MAX_STRING_SIZE)
    len=MAX_STRING_SIZE-1;
  else
    len=strlen(a->s);
  strncpy(s, a->s, len);
  *(s+len)='\0';
}

void FString_rp(FString *a, char *orig, char *repl) {
  char *ch;
  if (!(ch = strstr(a->s, orig)))
    return;
  FString *buffer=(FString *) malloc(sizeof(char *));
  
  buffer->s=(char *) malloc((strlen(a->s)+1-strlen(orig)+strlen(repl))*sizeof(char));
  strncpy(buffer->s, a->s, ch-a->s);
  buffer->s[ch-a->s] = 0;
  sprintf(buffer->s+(ch-a->s), "%s%s", repl, ch+strlen(orig));
  a->s=buffer->s;
  free(buffer);
}

typedef struct ENamedName
{
  int a;
} ENamedName;

typedef FString FName;

typedef struct FScannerSTCK {
  void (*Open) (const char *lumpname);
  void (*OpenFile) (const char *filename);
  void (*OpenMem) (const char *name, const char *buffer, int size);
  void (*OpenLumpNum) (int lump);
  void (*Close) ();

  void (*SetCMode) (short cmode);
  void (*SetEscape) (short esc);
  const SavedPos (*SavePos) ();
  void (*RestorePos) (SavedPos *pos);

  FString (*TokenName) (int token, const char *string);

  short (*GetString) ();
  void (*MustGetString) ();
  void (*MustGetStringName) (const char *name);
  short (*CheckString) (const char *name);

  short (*GetToken) ();
  void (*MustGetAnyToken) ();
  void (*TokenMustBe) (int token);
  void (*MustGetToken) (int token);
  short (*CheckToken) (int token);
  short (*CheckTokenId) (ENamedName id);

  short (*GetNumber) ();
  void (*MustGetNumber) ();
  short (*CheckNumber) ();

  short (*GetFloat) ();
  void (*MustGetFloat) ();
  short (*CheckFloat) ();

  void (*UnGet) ();

  short (*Compare) (const char *text);
  int (*MatchString) (const char * const *strings, size_t stride);
  int (*MustMatchString) (const char * const *strings, size_t stride);
  int (*GetMessageLine) ();

  void (*ScriptError) (const char *message, ...);
  void (*ScriptMessage) (const char *message, ...);

  short (*isText) ();

  // Members ------------------------------------------------------
  char *String;
  int StringLen;
  int TokenType;
  int Number;
  double Float;
  FName Name;
  int Line;
  short End;
  short Crossed;
  int LumpNum;
  FString ScriptName;

  void (*PrepareScript) ();
  void (*CheckOpen) ();
  short (*ScanString) (short tokens);

  short ScriptOpen;
  FString ScriptBuffer;
  const char *ScriptPtr;
  const char *ScriptEndPtr;
  char StringBuffer[MAX_STRING_SIZE];
  FString *BigStringBuffer;
  short AlreadyGot;
  int AlreadyGotLine;
  short LastGotToken;
  const char *LastGotPtr;
  int LastGotLine;
  short CMode;
  short Escape;


  int a;
  void (*fun) (int * a);
} FScannerSTCK;

enum
{
  TK_SequenceStart = 256,
#define xx(sym,str) sym,
#include "sc_man_tokens.h"
  TK_LastToken
};

enum
{
  MSG_WARNING,
  MSG_FATAL,
  MSG_ERROR,
  MSG_DEBUG,
  MSG_LOG,
  MSG_DEBUGLOG,
  MSG_MESSAGE
};

struct FScriptPositionSTCK;

typedef struct FScriptPositionSTCK
{
  int ErrorCounter;
  FString FileName;
  int ScriptLine;

} FScriptPositionSTCK;

void FScriptPosition_Message (FScriptPositionSTCK *self, TTF_Font *fnt, int *chg, size_t *tsl, SDL_Surface **text_solid, int severity, size_t max, const char *message, ...)
{
  char* format;

//if ((severity == MSG_DEBUG || severity == MSG_DEBUGLOG)) return;

  if (message == NULL)
  {
     snprintf(format,max,"Bad syntax.");
  }
  else
  {
      va_list arglist;
      va_start (arglist, message);
      vsnprintf(format,max,message,arglist);
      va_end (arglist);
  }

  const char *type = "";
  //int level = PRINT_HIGH;
  SDL_Color c_yel = TEXTCOLOR_YELLOW;
  SDL_Color c_red = TEXTCOLOR_RED;
  SDL_Color c_grn = TEXTCOLOR_GREEN;
  SDL_Color *color;
  color=(SDL_Color *) malloc(COBJ_INTSIZE*sizeof(int));

  switch (severity)
  {
  case MSG_WARNING:
	  type = "warning";
	  color = &c_yel;
	  break;

  case MSG_ERROR:
	  self->ErrorCounter++;
	  type = "error";
	  color = &c_red;
	  break;

  case MSG_MESSAGE:
  case MSG_DEBUG:
	  type = "message";
	  color = &c_grn;
	  break;

  case MSG_DEBUGLOG:
  case MSG_LOG:
	  type = "message";
//	  level = PRINT_LOG;
	  color = (SDL_Color*) NULL;
	  break;

  case MSG_FATAL:
	  fprintf (stderr,"Script error, \"%s\" line %d:\n%s\n",
		  self->FileName.s, self->ScriptLine, format);
	  return;
  default:
	  color = &c_yel;
	  return;
  }
  int j=*tsl;
  *tsl=*tsl+1;
  text_solid = (SDL_Surface **)realloc(text_solid, *tsl * sizeof( SDL_Surface * ) );
  *(text_solid+j) = (SDL_Surface *)malloc( sizeof( SDL_Surface ) );
  text_solid[j]=TTF_RenderUTF8_Blended(fnt, format, *color);
  *chg=1;
  /*Printf (level, "%sScript %s, \"%s\" line %d:\n%s%s\n",
	  color, type, FileName.GetChars(), ScriptLine, color, composed.GetChars());*/
}

void FScriptPosition_ResetErrorCounter(struct FScriptPositionSTCK *this)
{
  this->ErrorCounter = 0;
  return;
}

FScriptPositionSTCK *FScriptPosition_(void)
{
  FScriptPositionSTCK *this=malloc(sizeof(FScriptPositionSTCK));
  this->ScriptLine=0;
  return this;
}

void FScriptPosition_Op(FScriptPositionSTCK *this,FScriptPositionSTCK *other)
{
  this->FileName = other->FileName;
  this->ScriptLine = other->ScriptLine;
  return;
}

FScriptPositionSTCK *FScriptPosition_O(FScriptPositionSTCK *other)
{
  FScriptPositionSTCK *this=malloc(sizeof(FScriptPositionSTCK));
  FScriptPosition_Op(this,other);
  return this;
}

void FScriptPosition_NL(FScriptPositionSTCK *this, FString fname, int line)
{
  this->FileName=fname;
  this->ScriptLine=line;
  return;
}

FScriptPositionSTCK *FScriptPosition_NL_(FString fname, int line)
{
  FScriptPositionSTCK *this=malloc(sizeof(FScriptPositionSTCK));
  FScriptPosition_NL(this,fname,line);
  return this;
}

void FScriptPosition_SC(FScriptPositionSTCK *this, FScannerSTCK *sc)
{
  this->FileName = sc->ScriptName;
  this->ScriptLine = sc->GetMessageLine();
  return;
}

FScriptPositionSTCK *FScriptPosition_SC_(FScannerSTCK *sc)
{
  FScriptPositionSTCK *this=malloc(sizeof(FScriptPositionSTCK));
  FScriptPosition_SC(this,sc);
  return this;
}

FScannerSTCK *FScanner_(void) //хаха и тут наебал)
{
  FScannerSTCK *this=malloc(sizeof(FScannerSTCK));
  this->ScriptOpen = 0;
  return this;
}

void FScanner_Op(FScannerSTCK *this,FScannerSTCK *other)
{
  /*if (*this == *other) В C нету сравнения структур, только в c++. А значит, будем исходить из предположения, что они различаются и... В случае, если они были равны, мы сделаем не так много лишней работы, как если будем сравнивать все значения (коих стопицот)
  {
    return;
  }*/
  if (!other->ScriptOpen)
  {
    this->Close();
    return;
  }
  this->ScriptOpen = 1;
  this->ScriptName = other->ScriptName;
  this->ScriptBuffer = other->ScriptBuffer;
  this->ScriptPtr = other->ScriptPtr;
  this->ScriptEndPtr = other->ScriptEndPtr;
  this->AlreadyGot = other->AlreadyGot;
  this->AlreadyGotLine = other->AlreadyGotLine;
  this->LastGotToken = other->LastGotToken;
  this->LastGotPtr = other->LastGotPtr;
  this->LastGotLine = other->LastGotLine;
  this->CMode = other->CMode;
  this->Escape = other->Escape;

  // Copy public members
  if (other->String == other->StringBuffer)
  {
	  memcpy(this->StringBuffer, other->StringBuffer, sizeof(this->StringBuffer));
	  FString_v(this->BigStringBuffer,"");
	  this->String = this->StringBuffer;
  }
  else
  {
  	  this->BigStringBuffer = other->BigStringBuffer;
	  free(this->String);
	  this->String=(char *) malloc( MAX_STRING_SIZE * sizeof( char ) ); //ура!
  	  FString_tc(this->String,this->BigStringBuffer); //бамс, нахуя сложно, если мона просто
  }
  this->StringLen = other->StringLen;
  this->TokenType = other->TokenType;
  this->Number = other->Number;
  this->Float = other->Float;
  this->Name = other->Name;
  this->Line = other->Line;
  this->End = other->End;
  this->Crossed = other->Crossed;
  return;
}

FScannerSTCK *FScanner_C(FScannerSTCK *other) //хаха и тут наебал)
{
  FScannerSTCK *this=malloc(sizeof(FScannerSTCK));
  FScanner_Op(this,other);
//  FScannerSTCK *this=other;
  this->ScriptOpen = 0;
  return this;
}

FScannerSTCK *FScanner_L(int lumpnum) //хаха и тут наебал)
{
  FScannerSTCK *this=malloc(sizeof(FScannerSTCK));
  this->OpenLumpNum(lumpnum);
  this->ScriptOpen = 0;
  return this;
}

/*	FScanner();
	FScanner(const FScanner &other);
	FScanner(int lumpnum);
	~FScanner();*/
