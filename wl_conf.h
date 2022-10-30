// WL_CONF.H

//===========================================================================

#define MAX_BUF_LENGTH      150
#define MAX_LIN_LENGTH      100
#define MAX_STR_LENGTH      50

typedef struct
{    
    int char_index;
    int char_count;
    int cfg_number;
    char *cfg_string; 
} gameConfig; 

extern       gameConfig  gameVars;

extern int   gamefile;

extern int   cfg_lineNumber; 
extern char  *cfg_line;
extern char  *cfg_buffer;
extern int   buffer_index;
extern int   buffer_total;

int ReadLine(void);
int ReadNumber (void);
int LoadFileBuffer (void);
int IgnoreEmptySpace (void);
int ReadString (void);

//===========================================================================


