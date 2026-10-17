#include "Protocol.h"
#include <ctype.h>
#include <string.h>
namespace Diyz {
const char* errorName(ParseResult e){ switch(e){case ParseResult::EMPTY:return "EMPTY_COMMAND";case ParseResult::TOO_LONG:return "LINE_TOO_LONG";case ParseResult::INVALID_SYNTAX:return "INVALID_SYNTAX";case ParseResult::INVALID_ID:return "INVALID_ID";default:return "NONE";} }
ParseResult parseCommand(const char* line, Command& out){
 out=Command{}; if(!line)return ParseResult::EMPTY;
 const size_t n=strlen(line); if(n>=MAX_LINE)return ParseResult::TOO_LONG;
 char buf[MAX_LINE]; memcpy(buf,line,n+1);
 char* save=nullptr; char* tok=strtok_r(buf," \t",&save); if(!tok)return ParseResult::EMPTY;
 const size_t len=strlen(tok); if(len==0||len>=sizeof(out.verb))return ParseResult::INVALID_SYNTAX;
 for(size_t i=0;i<len;i++){if(!isalpha((unsigned char)tok[i])&&tok[i]!='?')return ParseResult::INVALID_SYNTAX;out.verb[i]=(char)toupper((unsigned char)tok[i]);}
 while((tok=strtok_r(nullptr," \t",&save))){
   if(strncmp(tok,"ID=",3)!=0)return ParseResult::INVALID_SYNTAX;
   if(out.hasId)return ParseResult::INVALID_ID;
   const char* val=tok+3; size_t k=strlen(val);
   if(k==0||k>=sizeof(out.id))return ParseResult::INVALID_ID;
   for(size_t i=0;i<k;i++)if(!isalnum((unsigned char)val[i])&&val[i]!='_'&&val[i]!='-')return ParseResult::INVALID_ID;
   memcpy(out.id,val,k+1);out.hasId=true;
 }
 return ParseResult::OK;
}
}
