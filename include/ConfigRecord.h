#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
namespace Diyz {
static constexpr uint32_t CONFIG_MAGIC=0x44595A31UL; // DYZ1
static constexpr uint32_t CONFIG_SCHEMA=1;
struct ConfigRecord {uint32_t magic;uint32_t schema;uint32_t sequence;uint32_t reserved;uint32_t crc;};
static_assert(sizeof(ConfigRecord)==20,"ConfigRecord binary size must be 20");
inline uint32_t crc32(const uint8_t* data,size_t len){
 uint32_t crc=0xFFFFFFFFUL;
 for(size_t i=0;i<len;++i){crc^=data[i];for(int j=0;j<8;++j)crc=(crc>>1)^((crc&1)?0xEDB88320UL:0UL);}
 return ~crc;
}
inline uint32_t recordCrc(const ConfigRecord& rec){return crc32(reinterpret_cast<const uint8_t*>(&rec),offsetof(ConfigRecord,crc));}
inline ConfigRecord makeRecord(uint32_t seq){ConfigRecord r{CONFIG_MAGIC,CONFIG_SCHEMA,seq,0,0};r.crc=recordCrc(r);return r;}
inline bool validRecord(const ConfigRecord& r){return r.magic==CONFIG_MAGIC && r.schema==CONFIG_SCHEMA && r.reserved==0 && r.crc==recordCrc(r);}
inline bool laterSequence(uint32_t a,uint32_t b){return (int32_t)(a-b)>0;}
}
