#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
/* data */

typedef struct erow {
  int idx;
  int size;
  int rsize;//rendersize
  char* chars;
  char* render;
  unsigned char* hl;
  int hl_open_comment;
}erow;
//future use
typedef struct color{
  int r;
  int g;
  int b;
}color;

enum editorHighlight{
  HL_NORMAL=0,
  HL_NONPRINT,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_STRING,
  HL_NUMBER,
  HL_SEP,
  HL_MATCH
};

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)
struct editorSyntax{
  char* filetype;
  char** filematch;
  char** keywords;
  char* singleline_comment_start;
  char* multiline_comment_start;
  char* multiline_comment_end;
  int flags;
};
struct editorConfig{
  int T_V;
  int rowoff;
  int coloff;
  int cx,cy;
  int rx;
  int screenrows;
  int screencols;
  int numrows;
  erow *row;
  int dirty;
  struct termios orig_termios;
  char* filename;
  char statusmsg[80];
  time_t statusmsg_time;
  char dflt[20];
  char digits[20];
  char strings[20];
  char match[20];
  char comments[20];
  char keywords1[20];
  char keywords2[20];
  char sep[20];
  char bg[20];
  char nbg[20];
  char np[20];
  struct editorSyntax *syntax;
};
struct editorConfig E;

/*file-types*/
//C/C++
char* C_HL_extensions[]={".c",".h",".cpp",".hpp",NULL};
char* PY_HL_extensions[]={".py",".ipynb",".pyc",".pyo",".pyw",".pyz",".pyd",".pyi",".pyx",NULL};
char* JS_HL_extensions[]={".js",".jsx",".ts",".tsx","mjs","cjs",NULL};
char* JV_HL_extensions[]={".java",".class",".jar",".war",".ear",NULL};
char* RS_HL_extensions[]={".rs",".rlib",".so",".dylib",NULL};
char* GO_HL_extensions[]={".go",NULL};
char* BIN_HL_extension[]={".exe",".dll",".a",NULL};
char* ML_HL_extensions[]={".toml",".yaml",".xml",".html",".json",NULL};
char* CFG_HL_extensions[]={".cfg",".conf",".config",".ini",NULL};
//will implement new highlight type for all included libraries later
char* C_HL_keywords[]={
  "switch","if","while","for","break","continue","return","else",
  "#include","#define","assert.h|",
  "complex.h|", "unistd.h|","termios.h|","fcntl.h|","sys/ioctl.h|","sys/types.h|","ctype.h|",  "errno.h|",  "fenv.h|","float.h|",  
  "inttypes.h|",  "iso646.h|",  "limits.h|",  "locale.h|",  "math.h|",  "setjmp.h|",  "signal.h|","stdalign.h|",  "stdarg.h|",  "stdatomic.h|",  
  "stdbool.h|",  "stddef.h|",  "stdint.h|",  "stdio.h|",  "stdlib.h|",  "stdnoreturn.h|",  "string.h|",  "tgmath.h|",  "threads.h|",  "time.h|",  
  "uchar.h|",  "wchar.h|","wctype.h|","struct","union","typedef","static","enum","class","case",
  "int|","uint|","long|","double|","float|","char|","unsigned|","signed|","void|", NULL};
char* PY_HL_keywords[]={"def","return","if","for","in","import","as","from","class","with",
        "int|","float|","str|","list|","dict|","tuple|","set|","bool|",
	"numpy|","pandas|","requests|","os|","sys|","matplotlib|","scikit-learn|","tensorflow|","django|","flask|",
	NULL};
char* JS_HL_keywords[]={"break","case","catch","class","const","continue","debugger","default","delete","do","else",
  "export","extends","false","finally","for","function","if","import","in","instanceof","new","null","return","super","switch","console",
  "this","throw","true","try","typeof","var","void","while","with","yield","await","enum","implements","interface","let","package","private",
  "protected","public","static","number|","string|","boolean|","null|","undefined|","symbol|","bigint|","object|","function|",NULL};
char* JV_HL_keywords[]={"abstract","assert","boolean","break","byte","case","catch","char","class","const","continue","default","do","double",
  "else","enum","extends","final","finally","float","for","goto","if","implements","import","instanceof","int","interface","long","native","new",
  "package","private","protected","public","return","short","static","strictfp","super","switch","synchronized","this","throw","throws","transient",
  "try","void","volatile","while","byte|","short|","int|","long|","float|","double|","boolean|","char|","String|","Array|","Class|","Interface|","Enum|",NULL};
char* RS_HL_keywords[]={"as","async","await","break","const","continue","crate","dyn","else","enum","extern","false","fn","for","if","impl","in","let","loop",
  "match","mod","move","mut","pub","ref","return","Self","self","static","struct","super","trait","true","type","union","unsafe","use","where","while",
  "i8|","i16|","i32|","i64|","i128|","isize|","u8|","u16|","u32|","u64|","u128|","usize|","f32|","f64|","bool|","char|","Tuple|","Array|","Vector|",NULL};
char* GO_HL_keywords[]={"break","case","chan","const","continue","defer","else","fallthrough","for","func","go","goto","if","import","interface","map",
  "package","range","return","select","struct","switch","type","var","int|","int8|","int16|","int32|","int64|","uint|","uint8|","uint16|","uint32|","uint64|",
  "uintptr|","float32|","float64|","complex64|","complex128|","bool|","string|","byte|","rune|","array|","slice|","map|","struct|","pointer|","function|",
  "channel|",NULL};
char* BIN_HL_keywords[]={NULL};
char* ML_HL_keywords[]={NULL};
char* CFG_HL_keywords[]={NULL};
struct editorSyntax HLDB[]={
  {
    "c",
    C_HL_extensions,
    C_HL_keywords,
    "//",
    "/*",
    "*/",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  { "python",
    PY_HL_extensions,
    PY_HL_keywords,
    "#",
    "/*",
    "*/",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {
  "java",
  JV_HL_extensions,
  JV_HL_keywords,
  "//",
  "/*",
  "*/",
  HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {"javascript/typescript",
  JS_HL_extensions,
  JS_HL_keywords,
  "//",
  "/*",
  "*/",
  HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {"Rust",
  RS_HL_extensions,
  RS_HL_keywords,
  "//",
  "/*",
  "*/",
  HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {"Go",
  GO_HL_extensions,
  GO_HL_keywords,
  "//",
  "/*",
  "*/",
  HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {"Binary",
  BIN_HL_extension,
  BIN_HL_keywords,
  "#",
  "~",
  "~",
  HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {"Markup",
  ML_HL_extensions,
  ML_HL_keywords,
  "#",
  "<!--",
  "-->",
  HL_HIGHLIGHT_NUMBERS|HL_HIGHLIGHT_STRINGS
  },
  {"Config",
  CFG_HL_extensions,
  CFG_HL_keywords,
  "#",
  "/*",
  "*/",
  HL_HIGHLIGHT_NUMBERS|HL_HIGHLIGHT_STRINGS
  },
};
#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))
/*proto*/
void setStatusMessage(const char* fmt,...);
void refreshScreen();
char* editorPrompt(char* prompt,void(*callback)(char*,int));

/*def*/
#define CTRL_KEY(k) ((k) & 0x1f)
#define XET_VERSION "0.1.0"

enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};



/* terminal */

void die(const char *s){
  perror(s);
  exit(1);
}

void disableRawMode(){
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios)==-1)
	  die("tcsetattr");
}

void enableRawMode(){
  if(tcgetattr(STDIN_FILENO, &E.orig_termios)==-1) 
	  die("tcsetattr");
  atexit(disableRawMode);
  struct termios raw=E.orig_termios;
  raw.c_lflag &=~(ECHO | ICANON | ISIG | IEXTEN);
  raw.c_iflag &=~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &=~(OPOST);
  raw.c_cflag |=(CS8);
  raw.c_cc[VMIN]=0;
  raw.c_cc[VTIME]=1;
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)==-1)
	  die("tcsetattr");
}

int readKey(){
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if(nread == -1 && errno != EAGAIN) die("read");
  }
    if (c == '\x1b') {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == '['){
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
	    case '3': return DEL_KEY;
	    case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } 
    else{
      switch (seq[1]) {
        case 'A': return ARROW_UP;
        case 'B': return ARROW_DOWN;
        case 'C': return ARROW_RIGHT;
        case 'D': return ARROW_LEFT;
	case 'H': return HOME_KEY;
	case 'F': return END_KEY;
      }
    }
    }
    else if(seq[0]=='0'){
      switch(seq[1]){
	case 'H':return HOME_KEY;
	case 'F':return END_KEY;
      }
    }
    return '\x1b';
  } else {
  return c;}
}

int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i=0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  while (i<sizeof(buf)-1){
    if(read(STDIN_FILENO, &buf[i],1)!=1) break;
    if(buf[i]=='R') break;
    i++;
  }
  buf[i]='\0';
  if (buf[i]!='\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d",rows,cols)!=2) return -1;
  readKey();
  return 0;
}

int getWindowSize(int *rows, int *cols){
  struct winsize ws;
  if(ioctl(STDOUT_FILENO,TIOCGWINSZ, &ws)==-1 || ws.ws_col==0){
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) 
	    return -1;
    return getCursorPosition(rows, cols);
    return -1;}
  else{
    *cols=ws.ws_col;
    *rows= ws.ws_row;
    return 0;
  }
}
/*syntax highlighting*/
int is_separator(int c){
  return isspace(c)||c=='\0'||
	  strchr(",.()+-/*=`~&?<>[]{};:",c)!=NULL;
}
void createAnsiColor(char *dest, int r, int g, int b) {
  snprintf(dest,20,"\x1b[38;2;%d;%d;%dm",r,g,b);
}
void createBgColor(char* dest,int r,int g, int b){
  snprintf(dest,20,"\x1b[48;2;%d;%d;%dm",r,g,b);
}

// Function to parse a line and extract RGB values
int parseLine(char *line, char *key, int *r, int *g, int *b) {
    char *equals_pos;
    char *comma1, *comma2;
    while (*line == ' ' || *line == '\t') {
        ++line;
        if(*line=='\n'||*line=='\0'||*line=='#'){
          return 0;
        }
    }

    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') {
        return 0;
    }
    // Find the '=' character
    equals_pos = strchr(line, '=');
    if (!equals_pos) {
        return 0;
    }
    
    // Extract key part
    int key_len = equals_pos - line;
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    
    // Remove any trailing spaces from key
    while (key_len > 0 && key[key_len-1] == ' ') {
        key[--key_len] = '\0';
    }
    
    // Move to the value part (after '=')
    char *value_start = equals_pos + 1;
    
    // Find first comma
    comma1 = strchr(value_start, ',');
    if (!comma1) return 0;
    
    // Find second comma
    comma2 = strchr(comma1 + 1, ',');
    if (!comma2) return 0;
    
    // Parse RGB values manually
    *r = atoi(value_start);
    *g = atoi(comma1 + 1);
    *b = atoi(comma2 + 1);
    if(*r>255) *r=255; 
    if(*r<0) *r=0;
    if(*g>255) *g=255; 
    if(*g<0) *g=0;
    if(*b>255) *b=255; 
    if(*b<0) *b=0;
    
    return 1;
}
void setColor(){
  strcpy(E.dflt,"\x1b[38;2;180;180;180m");
  strcpy(E.digits,"\x1b[38;2;050;125;250m");
  strcpy(E.strings,"\x1b[38;2;050;250;100m");
  strcpy(E.match,"\x1b[38;2;250;075;250m");
  strcpy(E.comments,"\x1b[38;2;100;100;100m");
  strcpy(E.keywords1,"\x1b[38;2;250;160;010m");
  strcpy(E.keywords2,"\x1b[38;2;250;100;180m");
  strcpy(E.sep,"\x1b[38;2;250;240;010m");
  strcpy(E.bg,"\x1b[48;2;050;025;075m");
  strcpy(E.nbg,"\x1b[48;2;200;010;010m");
  strcpy(E.np,"\x1b[38;2;010;025;025m");
  FILE *file;
    char line[256];
    char key[50];
    int r, g, b;
    
    // Open the configuration file
    file = fopen("colors.config", "r");
    if (!file) {
        printf("Error: Could not open colors.config file\n");
        return;
    }
    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        //printf("Debug: Reading line: %s", line);
        if (parseLine(line, key, &r, &g, &b)) {
            //printf("Debug: Parsed key='%s', RGB=(%d,%d,%d)\n", key, r, g, b);
            
            // Match the key and assign to appropriate structure member
            if (strcmp(key, "default-color") == 0) {
                createAnsiColor(E.dflt, r, g, b);
                //printf("Debug: Set default_color\n");
            }
            else if (strcmp(key, "number-color") == 0) {
                createAnsiColor(E.digits, r, g, b);
                //printf("Debug: Set number\n");
            }
            else if (strcmp(key, "string-color") == 0) {
                createAnsiColor(E.strings, r, g, b);
                //printf("Debug: Set string\n");
            }
            else if (strcmp(key, "find-color") == 0) {
                createAnsiColor(E.match, r, g, b);
                //printf("Debug: Set find\n");
            }
            else if (strcmp(key, "comment-color") == 0) {
                createAnsiColor(E.comments, r, g, b);
                //printf("Debug: Set comment\n");
            }
            else if (strcmp(key, "keyword-color") == 0) {
                createAnsiColor(E.keywords1, r, g, b);
                //printf("Debug: Set keyword\n");
            }
            else if (strcmp(key, "type-color") == 0) {
                createAnsiColor(E.keywords2, r, g, b);
                //printf("Debug: Set type\n");
            }
            else if (strcmp(key, "separator-color") == 0) {
                createAnsiColor(E.sep, r, g, b);
                //printf("Debug: Set separator\n");
            }
            else if (strcmp(key, "background-color") == 0) {
                createBgColor(E.bg, r, g, b);
                //printf("Debug: Set background\n");
            }
            else if (strcmp(key, "exception-color") == 0) {
                createBgColor(E.nbg, r, g, b);
                //printf("Debug: Set exception\n");
            }
            else if (strcmp(key, "nonprint-color") == 0) {
                createAnsiColor(E.np, r, g, b);
                //printf("Debug: Set nonprint\n");
            }       
    }
  }
  fclose(file);
}

void updateSyntax(erow* row){
  row->hl = realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);
  if (E.syntax == NULL) return;
  char **keywords = E.syntax->keywords;
  char *scs = E.syntax->singleline_comment_start;
  char *mcs = E.syntax->multiline_comment_start;
  char *mce = E.syntax->multiline_comment_end;
  int scs_len = scs ? strlen(scs) : 0;
  int mcs_len = mcs ? strlen(mcs) : 0;
  int mce_len = mce ? strlen(mce) : 0;
  int prev_sep = 1;
  int in_string = 0;
  int in_comment = (row->idx>0&&
		  E.row[row->idx-1].hl_open_comment);
  int i = 0;
  while (i < row->rsize) {
    //row->hl[i]=HL_NORMAL;
    char c = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;
    if(is_separator(c)&&!in_comment&&!in_string){
      row->hl[i]=HL_SEP;
    }
    if (scs_len && !in_string && !in_comment) {
      if (!strncmp(&row->render[i], scs, scs_len)) {
        memset(&row->hl[i], HL_COMMENT, row->rsize - i);
        break;
      }
    }
    if (mcs_len && mce_len && !in_string) {
      if (in_comment) {
        row->hl[i] = HL_MLCOMMENT;
        if (!strncmp(&row->render[i], mce, mce_len)) {
          memset(&row->hl[i], HL_MLCOMMENT, mce_len);
          i += mce_len;
          in_comment = 0;
          prev_sep = 1;
          continue;
        } else {
          i++;
          continue;
        }
      } 
      else if (!strncmp(&row->render[i], mcs, mcs_len)) {
        memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
        i += mcs_len;
        in_comment = 1;
        continue;
      }
    }
   
    if (E.syntax->flags & HL_HIGHLIGHT_STRINGS) {
      if (in_string) {
        row->hl[i] = HL_STRING;
        if (c == '\\' && i + 1 < row->rsize) {
          row->hl[i+1] = HL_STRING;
          i += 2;
          continue;
        }
        if (c == in_string) in_string = 0;
        i++;
        prev_sep = 1;
        continue;
      } 
      else {
        if (c == '"' || c == '\'') {
          in_string = c;
          row->hl[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }
    if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
      if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) ||
          (c == '.' && prev_hl == HL_NUMBER)) {
        row->hl[i] = HL_NUMBER;
        i++;
        prev_sep = 0;
        continue;
      }
    }
    if (prev_sep) {
      int j;
      for (j = 0; keywords[j]; j++) {
        int klen = strlen(keywords[j]);
        int kw2 = keywords[j][klen - 1] == '|';
        if (kw2) klen--;
        if (!strncmp(&row->render[i], keywords[j], klen) &&
            is_separator(row->render[i + klen])) {
          memset(&row->hl[i], 
			  kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          i += klen;
          break;
        }
      }
      if (keywords[j] != NULL) {
        prev_sep = 0;
        continue;
      }
    }
    prev_sep = is_separator(c);
    i++;
  }
  int changed = (row->hl_open_comment != in_comment);
  row->hl_open_comment = in_comment;
  if (changed && row->idx + 1 < E.numrows)
    updateSyntax(&E.row[row->idx + 1]);
}
//future use
/*
color syntaxToColor(int hl){  
  color output;
  output.r=200;output.g=200;output.b=200;
  switch(hl){
    case HL_NUMBER:
		    output.r=50;
		    output.g=125;
		    output.b=200;
		    return output;
    case HL_MATCH:
		    output.r=200;
		    output.g=50;
		    output.b=200;
		    return output;
    case HL_NORMAL:
    default:
		    output.r=200;
		    output.g=200;
		    output.b=200;
		    return output;
  }
}
*/
void selectSyntaxHighlight(){
  E.syntax = NULL;
  if (E.filename == NULL) return;
  char *ext = strrchr(E.filename, '.');
  for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
    struct editorSyntax *s = &HLDB[j];
    unsigned int i = 0;
    while (s->filematch[i]) {
      int is_ext = (s->filematch[i][0] == '.');
      if ((is_ext && ext && !strcmp(ext, s->filematch[i])) ||
          (!is_ext && strstr(E.filename, s->filematch[i]))) {
        E.syntax = s;
	int filerow;
	for(filerow=0;filerow<E.numrows;filerow++){
          updateSyntax(&E.row[filerow]);
	}
        return;
      }
      i++;
    }
  }
}


/*rowOp*/

int rowCxToRx(erow* row,int cx){
  int rx=0;
  int j;
  for(j=0;j<cx;j++){
  if (row->chars[j]=='\t')
	  rx+=(E.T_V-1)-(rx%E.T_V);
  rx++;
  }
  return rx;
}
int rowRxToCx(erow* row, int rx){
  int cur_rx=0;
  int cx;
  for(cx=0;cx<row->size;cx++){
    if(row->chars[cx]=='\t')
	    cur_rx+=(E.T_V-1)-(cur_rx%E.T_V);
    cur_rx++;
    if(cur_rx>rx) return cx;
  }
  return cx;
}

void updateRow(erow *row) {
  int tabs = 0;
  int j;
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t') tabs++;
  free(row->render);
  row->render = malloc(row->size + tabs*(E.T_V-1) + 1);
  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      row->render[idx++] = ' ';
      while (idx % E.T_V != 0) row->render[idx++] = ' ';
    } else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
  updateSyntax(row);
}


void insertRow(int at,char* s, size_t len){
  if(at<0||at>E.numrows) return;
  E.row=realloc(E.row,sizeof(erow)*(E.numrows+1));
  memmove(&E.row[at+1],&E.row[at],sizeof(erow)*(E.numrows-at));
  for(int j=at+1;j<=E.numrows;j++) E.row[j].idx++;
  E.row[at].idx=at;
  E.row[at].size=len;
  E.row[at].chars=malloc(len+1);
  memcpy(E.row[at].chars,s,len);
  E.row[at].chars[len]='\0';
  E.row[at].rsize=0;
  E.row[at].render=NULL;
  E.row[at].hl=NULL;
  E.row[at].hl_open_comment=0;
  updateRow(&E.row[at]);
  E.numrows++;
  E.dirty++;
}
void freeRow(erow *row) {
  free(row->render);
  free(row->chars);
  free(row->hl);
}
void delRow(int at) {
  if (at < 0 || at >= E.numrows) return;
  freeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], 
		  sizeof(erow) * (E.numrows - at - 1));
  for(int j=at;j<E.numrows-1;j++) E.row[j].idx--;
  E.numrows--;
  E.dirty++;
}

void rowInsertChar(erow* row,int at,int c){
  if(at<0||at>row->size) at=row->size;
  row->chars=realloc(row->chars,row->size+2);
  memmove(&row->chars[at+1],&row->chars[at],row->size-at+1);
  row->size++;
  row->chars[at]=c;
  updateRow(row);
  E.dirty++;
}

void rowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  updateRow(row);
  E.dirty++;
}

void rowDelChar(erow* row,int at){
  if(at<0|| at>=row->size) return;
  memmove(&row->chars[at],&row->chars[at+1],row->size-at);
  row->size--;
  updateRow(row);
  E.dirty++;
}

/*editorOps*/
void insertChar(int c){
  if(E.cy==E.numrows){
    insertRow(E.numrows,"",0);
  }
  rowInsertChar(&E.row[E.cy],E.cx,c);
  E.cx++;
}
void insertNewline(){
  if(E.cx==0){
    insertRow(E.cy,"",0);
  }
  else{
    erow* row = &E.row[E.cy];
    insertRow(E.cy+1,&row->chars[E.cx],row->size-E.cx);
    row=&E.row[E.cy];
    row->size=E.cx;
    row->chars[row->size]='\0';
    updateRow(row);
  }
  E.cy++;
  E.cx=0;
}
void delChar(){
  if(E.cy==E.numrows) return;
  if(E.cx==0&&E.cy==0) return;
  erow *row = &E.row[E.cy];
  if(E.cx>0){
    rowDelChar(row,E.cx-1);
    E.cx--;
  }
  else{
    E.cx=E.row[E.cy-1].size;
    rowAppendString(&E.row[E.cy-1],row->chars,row->size);
    delRow(E.cy);
    E.cy--;
  }
}
/*file I/O*/
char* rowToString(int *buflen){
  int totlen=0;
  int j;
  for(j=0;j<E.numrows;j++)
	  totlen+=E.row[j].size+1;
  *buflen=totlen;
  char* buf=malloc(totlen);
  char* p=buf;
  for(j=0;j<E.numrows;j++){
    memcpy(p,E.row[j].chars,E.row[j].size);
    p+=E.row[j].size;
    *p='\n';
    p++;
  }
  return buf;
}

void Open(char* filename){
  free(E.filename);
  E.filename=strdup(filename);
  selectSyntaxHighlight();
  FILE* fp=fopen(filename,"r");
  if(!fp) die("open");
  char* line=NULL;
  size_t linecap=0;
  ssize_t linelen;
  while((linelen=getline(&line,&linecap,fp))!=-1){
    if(linelen&&(line[linelen-1]=='\n' ||
		      line[linelen-1]=='\r'))
	    line[--linelen]='\0';
    insertRow(E.numrows,line,linelen);
  }
  free(line);
  fclose(fp);
  E.dirty=0;
}
void Save(){
  if(E.filename==NULL){
    E.filename=editorPrompt("Save as: %s (ESC to cancel)",NULL);
    if(E.filename==NULL){
      setStatusMessage("Save aborted");
      return;
    }
    selectSyntaxHighlight();
  }
  int len;
  char* buf=rowToString(&len);
  int fd=open(E.filename,O_RDWR|O_CREAT,0644);
  if(fd!=-1){
    if(ftruncate(fd,len)!=-1){
      if(write(fd,buf,len)==len){
	      close(fd);
	      free(buf);
	      E.dirty=0;
	      setStatusMessage("%d byte written to disk",
			      len);
	      return;
      }
    }
    close(fd);
  }
  free(buf);
  setStatusMessage("Can't save! I/O error: %s",strerror(errno));
}

/*find*/
void findCallback(char* query,int key){
  static int last_match=-1;
  static int direction=1;
  static int saved_hl_line;
  static char *saved_hl = NULL;
  if (saved_hl) {
    memcpy(E.row[saved_hl_line].hl, saved_hl, 
		    E.row[saved_hl_line].rsize);
    free(saved_hl);
    saved_hl = NULL;
  }
  if(key=='\r'||key=='\x1b'){
    last_match=-1;
    direction=1;
    return;
  }
  else if(key==ARROW_RIGHT||key==ARROW_DOWN){
    direction=1;
  }
  else if(key==ARROW_LEFT||key==ARROW_UP){
    direction=-1;
  }
  else {
    last_match=-1;
    direction=1;
  }
  if(last_match==-1) direction=1;
  int current = last_match;
  int i;
  for(i=0;i<E.numrows;i++){
    current+=direction;
    if(current==-1) current=E.numrows-1;
    else if(current==E.numrows) current=0;
    erow* row =&E.row[current];
    char* match=strstr(row->render,query);
    if(match){
      last_match=current;
      E.cy=current;
      E.cy=i;
      E.cx=rowRxToCx(row,match-row->render);
      E.rowoff=E.numrows;
      saved_hl_line = current;
      saved_hl = malloc(row->rsize);
      memcpy(saved_hl, row->hl, row->rsize);
      memset(&row->hl[match-row->render],HL_MATCH,strlen(query));
      break;
    }
  }
}
void Find(){
  int saved_cx=E.cx;
  int saved_cy = E.cy;
  int saved_coloff = E.coloff;
  int saved_rowoff = E.rowoff;
  char* query=editorPrompt("Search: %s (use ESC/arrows/Enter)",
		  findCallback);
  if(query){
    free(query);
  }
  else{
    E.cx = saved_cx;
    E.cy = saved_cy;
    E.coloff = saved_coloff;
    E.rowoff = saved_rowoff;
  }
}

/*append buffer*/
struct abuf{
  char *b;
  int len;
};
#define ABUF_INIT {NULL,0}
void abAppend(struct abuf *ab, const char *s, int len){
  char *new = realloc (ab->b, ab->len+len);
  if(new==NULL) return;
  memcpy(&new[ab->len],s,len);
  ab->b=new;
  ab->len+=len;
}
void abFree(struct abuf *ab){
  free(ab->b);
}
/*input*/
char* editorPrompt(char* prompt,void (*callback)(char*, int)){
  size_t bufsize=128;
  char* buf = malloc(bufsize);
  size_t buflen=0;
  buf[0]='\0';
  while(1){
    setStatusMessage(prompt,buf);
    refreshScreen();
    int c = readKey();
    if(c==DEL_KEY||c==CTRL_KEY('h')||c==BACKSPACE){
      if(buflen!=0) buf[--buflen]='\0';
    }
    else if(c=='\x1b'){
      setStatusMessage("");
      if (callback) callback(buf,c);
      free(buf);
      return NULL;
    }
    else if(c=='\r'){
      if(buflen!=0){
        setStatusMessage("");
	if(callback) callback(buf,c);
	return buf;
      }
    }
    else if(!iscntrl(c)&&c<128){
      if(buflen==bufsize-1){
        bufsize*=2;
	buf=realloc(buf,bufsize);
      }
      buf[buflen++]=c;
      buf[buflen]='\0';
    }
    if(callback) callback(buf,c);
  }
}

void moveCursor(int key){
  erow* row = (E.cy>=E.numrows)?NULL:&E.row[E.cy];
  switch(key){
    case ARROW_LEFT:
     if(E.cx!=0) {E.cx--;}
     else if(E.cy>0) {E.cy--;E.cx=E.row[E.cy].size;}
      break;
    case ARROW_RIGHT:
      if(row&&E.cx<row->size) {E.cx++;}
      else if(row&&E.cx==row->size){
        E.cy++;
	E.cx=0;
      }
      break;
    case ARROW_UP:
      if(E.cy!=0) E.cy--;
      break;
    case ARROW_DOWN:
      if(E.cy<E.numrows) E.cy++;
      break;
  }
  row=(E.cy>=E.numrows)?NULL:&E.row[E.cy];
  int rowlen = row?row->size:0;
  if(E.cx>rowlen) E.cx=rowlen;
}

void processKeypress(){
  static int quit_confirm = 1;
  int c = readKey();
  switch (c) {
    case '\r':
      insertNewline();
      break;
    case CTRL_KEY('q'):
      if(E.dirty&&quit_confirm){
	setStatusMessage("WARNING!!! File has unsaved changes."
		"Press Ctrl-Q again to Quit.");
	quit_confirm--;
	return;
      }
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
    case CTRL_KEY('s'):
      Save();
      break;
    case HOME_KEY:
      E.cx=0;
      break;
    case END_KEY:
      if(E.cy<E.numrows) E.cx=E.row[E.cy].size;
      break;
    case CTRL_KEY('f'):
      Find();
      break;
    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
      if(c==DEL_KEY) moveCursor(ARROW_RIGHT);
      delChar();
      break;
    case PAGE_UP:
    case PAGE_DOWN:
      {
        if(c==PAGE_UP){
          E.cy=E.rowoff;
	}
	else if (c==PAGE_DOWN){
	  E.cy=E.rowoff+E.screenrows-1;
          if(E.cy>E.numrows) E.cy=E.numrows;
	}
        int times = E.screenrows;
        while (times--)
          moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      }
      break;
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      moveCursor(c);
      break;
    case CTRL_KEY('l'):
    case'\x1b':
      break;
    default:
      insertChar(c);
      break;
  }
  quit_confirm=1;
}

/*output*/

void scroll(){
  E.rx=0;
  if(E.cy<E.numrows){
    E.rx=rowCxToRx(&E.row[E.cy],E.cx);
  }
  if(E.cy<E.rowoff){
    E.rowoff=E.cy;
  }
  if(E.cy>=E.rowoff+E.screenrows){
    E.rowoff=E.cy-E.screenrows+1;
  }
  if(E.cx<E.coloff){
    E.coloff=E.rx;
  }
  if(E.cx>=E.coloff+E.screencols){
    E.coloff=E.rx-E.screencols+1;
  }
}

void drawRows(struct abuf *ab){
  //abAppend(ab,"\x1b[2J",4);  
  abAppend(ab,E.dflt,strlen(E.dflt));
  int y;
  for(y=0;y<E.screenrows;y++){
          int filerow=y+E.rowoff;
	  if(filerow>=E.numrows){
    if(y==E.screenrows/3&&E.numrows==0){
      char welcome[80];
      int welcomelen=snprintf(welcome,sizeof(welcome),
	"Xet editor --version %s", XET_VERSION);
      if(welcomelen>E.screencols) welcomelen=E.screencols;
      int padding = (E.screencols-welcomelen)/2;
      if(padding){
        abAppend(ab,"~",1);
	padding--;
      }
      while(padding--) abAppend(ab," ",1);
      abAppend(ab,welcome,welcomelen);
    }
    else{
      abAppend(ab,"~",1);
    }}
  else{
    int len=E.row[filerow].rsize-E.coloff;
    if(len<0) len=0;
    if(len>E.screencols) len=E.screencols;
    char* c =&E.row[filerow].render[E.coloff];
    unsigned char* hl=&E.row[filerow].hl[E.coloff];
    int j;
    for(j=0;j<len;j++){
      if(iscntrl(c[j])){
        char sym = (c[j]<=26)?'@'+c[j]:'?';
	abAppend(ab,E.nbg,strlen(E.nbg));
	abAppend(ab,E.np,strlen(E.np));
	abAppend(ab,&sym,1);
	abAppend(ab,E.bg,strlen(E.bg));
	abAppend(ab,E.dflt,strlen(E.dflt));
      }	    
      else if(hl[j]==HL_NORMAL){
        abAppend(ab,E.dflt,strlen(E.dflt));
	abAppend(ab,&c[j],1);
      }
      else if(hl[j]==HL_SEP){
        abAppend(ab,E.sep,strlen(E.sep));
	abAppend(ab,&c[j],1);
      }
      else{
	//color col=syntaxToColor(hl[j]);
	//future functionality
	//char buf[20];
	//int clen=snprintf(buf,sizeof(buf),"\x1b[38;2;%d;%d;%dm",col.r,col.g,col.b);
	//abAppend(ab,buf,clen);
	if(hl[j]==HL_NUMBER){
	  abAppend(ab,E.digits,strlen(E.digits));
          abAppend(ab,&c[j],1);
	}
	if(hl[j]==HL_SEP){
	  abAppend(ab,E.sep,strlen(E.sep));
	  abAppend(ab,&c[j],1);
	}
	if(hl[j]==HL_MATCH){
          abAppend(ab,E.match,strlen(E.match));
	  abAppend(ab,&c[j],1);
	}
	if(hl[j]==HL_STRING){
	  abAppend(ab,E.strings,strlen(E.strings));
	  abAppend(ab,&c[j],1);
	}
	if(hl[j]==HL_COMMENT||hl[j]==HL_MLCOMMENT){
          abAppend(ab,E.comments,strlen(E.comments));
	  abAppend(ab,&c[j],1);
	}

	if(hl[j]==HL_KEYWORD1){
          abAppend(ab,E.keywords1,strlen(E.keywords1));
	  abAppend(ab,&c[j],1);
	}
	if(hl[j]==HL_KEYWORD2){
	  abAppend(ab,E.keywords2,strlen(E.keywords2));
	  abAppend(ab,&c[j],1);
	}
      }
    }
    abAppend(ab,E.dflt,strlen(E.dflt));
    }
    abAppend(ab,"\x1b[K",3);
    abAppend(ab, "\r\n",2);
  }
}
void drawStatusBar(struct abuf* ab){
  abAppend(ab, "\x1b[7m",4);
  char status[80],rstatus[80];
  int len = snprintf(status,sizeof(status),"%.30s-%d lines %s",
		  E.filename?E.filename:"[No Name]",E.numrows,
		  E.dirty?"(modified)":"");
  int rlen=snprintf(rstatus,sizeof(rstatus),"%s | %d/%d",
	E.syntax?E.syntax->filetype:"no FT",E.cy+1,E.numrows);
  if(len>E.screencols) len= E.screencols;
  abAppend(ab,status,len);
  while(len<E.screencols){
    if(E.screencols-len==rlen){
      abAppend(ab,rstatus,rlen);
      break;
    }
    else{
      abAppend(ab,"-",1);
      len++;
    }
  }
  abAppend(ab,"\x1b[m",3);
  abAppend(ab,"\r\n",2);
}

void drawMessageBar(struct abuf* ab){
  abAppend(ab,"\x1b[K",3);
  int msglen=strlen(E.statusmsg);
  if(msglen>E.screencols) msglen= E.screencols;
  if(msglen&&time(NULL)-E.statusmsg_time<5)
	  abAppend(ab,E.statusmsg,msglen);
}

void refreshScreen(){
  scroll();
  struct abuf ab = ABUF_INIT;
  abAppend(&ab, "\x1b[?25l",6);
  abAppend(&ab, "\x1b[2J", 4);
  abAppend(&ab, "\x1b[H", 3);
  abAppend(&ab,E.bg,strlen(E.bg));
  drawRows(&ab);
  drawStatusBar(&ab);
  drawMessageBar(&ab);
  char buf[32];
  snprintf(buf,sizeof(buf), "\x1b[%d;%dH",(E.cy-E.rowoff)+1,
		  (E.rx-E.coloff)+1);
  abAppend(&ab,buf,strlen(buf));

  abAppend(&ab, "\x1b[?25h",6);
  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}
void setStatusMessage(const char* fmt,...){
  va_list ap;
  va_start(ap,fmt);
  vsnprintf(E.statusmsg,sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time=time(NULL);
}
/* init */
void initEditor(){
  E.T_V=8;
  E.rowoff=0;
  E.coloff=0;
  E.cx=0;
  E.cy=0;
  E.rx=0;
  E.numrows=0;
  E.row=NULL;
  E.dirty=0;
  E.filename=NULL;
  E.statusmsg[0]='\0';
  E.statusmsg_time=0;
  E.syntax=NULL;
  if(getWindowSize(&E.screenrows,&E.screencols)==-1)
		  die("getWindowSize");
  E.screenrows-=2;
  
}
int main(int argc,char *argv[]){
  enableRawMode();
  initEditor();
  setColor();
  if(argc>=2){
    Open(argv[1]);
  }
  setStatusMessage(
	"HELP: Ctrl-S = save | Ctrl-F = find | Ctrl-Q = quit");
  while(1){
    refreshScreen();
    processKeypress();
  }
  return 0;
}
