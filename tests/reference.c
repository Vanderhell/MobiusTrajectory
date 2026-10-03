#include "reference.h"
#include <stddef.h>
static const char rows[8][33]={"481E081FFDAB8273DA10C46B9C4C4EED","FC7C544B1173BBF3295C6715178DFC18","787E58B8248E651B14676D3BA604C804","CBA702E69866206E462F81E0336FED5E","0BFD4D2AE8A55FA92799264E8919ADDF","A068C3D6FCC77FAFC1E7B20810A43E49","F95CD1BBF3280124DBDBBDB541ABCC7F","A275EA898D21D192BFAE1C3A2E64C8E4"};
static uint8_t h(char c){return (uint8_t)(c<='9'?c-'0':c-'A'+10);}
static int bit(const uint8_t *p,uint32_t n){return (p[n/8u]&(uint8_t)(1u<<(n%8u)))!=0u;}
void reference_forward(uint8_t out[32],const uint8_t in[32])
{
 uint32_t cut,s; size_t j; for(j=0u;j<32u;++j)out[j]=0u;
 for(cut=0u;cut<256u;++cut) if(bit(in,cut)) {
  uint32_t x=cut&31u,y=0u,m=cut>>5;
  for(s=0u;s<64u;++s) { uint8_t v=h(rows[y][x]); uint32_t at=s/2u; if((s&1u)==0u)out[at]^=(uint8_t)(v<<4);else out[at]^=v; {uint32_t ny=(y+m+(x%2u))%8u;uint32_t nx=(x+1u)%32u;if(x==31u)ny=7u-ny;x=nx;y=ny;} }
 }
}
