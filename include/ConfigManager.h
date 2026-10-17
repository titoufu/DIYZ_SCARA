#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "ConfigRecord.h"
namespace Diyz {
enum class ConfigStorageState:uint8_t {UNINITIALIZED,OK,SCHEMA_ERROR,CRC_ERROR,IO_ERROR};
inline const char* configStorageName(ConfigStorageState s){
 switch(s){case ConfigStorageState::UNINITIALIZED:return "UNINITIALIZED";case ConfigStorageState::OK:return "OK";case ConfigStorageState::SCHEMA_ERROR:return "SCHEMA_ERROR";case ConfigStorageState::CRC_ERROR:return "CRC_ERROR";default:return "IO_ERROR";}
}
class ConfigManager {
 ConfigStorageState state_=ConfigStorageState::UNINITIALIZED;
 uint32_t sequence_=0; char active_='-';
 static bool load(Preferences& p,const char* key,ConfigRecord& r){return p.getBytesLength(key)==sizeof(r)&&p.getBytes(key,&r,sizeof(r))==sizeof(r);}
public:
 void begin(){
  state_=ConfigStorageState::UNINITIALIZED;sequence_=0;active_='-';
  Preferences p;
  if(!p.begin("diyz",true))return; // Namespace still absent. No writes on boot.
  ConfigRecord a{},b{}; const bool aExists=p.isKey("cfg_a"),bExists=p.isKey("cfg_b");
  const bool aRead=aExists&&load(p,"cfg_a",a),bRead=bExists&&load(p,"cfg_b",b);
  const bool av=aRead&&validRecord(a),bv=bRead&&validRecord(b);
  if(av||bv){bool useA=av&&(!bv||laterSequence(a.sequence,b.sequence));sequence_=useA?a.sequence:b.sequence;active_=useA?'A':'B';state_=ConfigStorageState::OK;}
  else if(aExists||bExists){
   const bool schema=(aRead&&a.magic==CONFIG_MAGIC&&a.schema!=CONFIG_SCHEMA)||(bRead&&b.magic==CONFIG_MAGIC&&b.schema!=CONFIG_SCHEMA);
   state_=schema?ConfigStorageState::SCHEMA_ERROR:ConfigStorageState::CRC_ERROR;
  }
  p.end();
 }
 // Explicit command only; never save from begin(). Dual slots protect last valid record.
 bool save(){
  Preferences p;if(!p.begin("diyz",false)){state_=ConfigStorageState::IO_ERROR;return false;}
  const char* key=(active_=='A')?"cfg_b":"cfg_a";
  const ConfigRecord next=makeRecord(sequence_+1);
  const bool ok=p.putBytes(key,&next,sizeof(next))==sizeof(next);
  ConfigRecord check{};const bool verified=ok&&load(p,key,check)&&validRecord(check)&&check.sequence==next.sequence;
  p.end();
  if(!verified){state_=ConfigStorageState::IO_ERROR;return false;}
  begin();return state_==ConfigStorageState::OK&&sequence_==next.sequence;
 }
 ConfigStorageState storageState()const{return state_;}
 bool storageOk()const{return state_==ConfigStorageState::OK;}
 uint32_t schema()const{return CONFIG_SCHEMA;}
 uint32_t sequence()const{return sequence_;}
 char activeSlot()const{return active_;}
};
}
