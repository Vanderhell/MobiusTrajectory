#include "mobius.h"
#include <stddef.h>

extern const uint8_t mobius_inverse_rows_v1[MOBIUS_BITS][MOBIUS_STATE_BYTES];

/* Rows are stored as high-to-low hex text, preserving the historical surface. */
static const char surface[8][33] = {
 "481E081FFDAB8273DA10C46B9C4C4EED","FC7C544B1173BBF3295C6715178DFC18",
 "787E58B8248E651B14676D3BA604C804","CBA702E69866206E462F81E0336FED5E",
 "0BFD4D2AE8A55FA92799264E8919ADDF","A068C3D6FCC77FAFC1E7B20810A43E49",
 "F95CD1BBF3280124DBDBBDB541ABCC7F","A275EA898D21D192BFAE1C3A2E64C8E4"
};
static uint8_t nibble(char c) { return (uint8_t)(c <= '9' ? c-'0' : c-'A'+10); }
static bool get(const uint8_t *v, uint32_t b) { return (v[b/8u] & (uint8_t)(1u << (b%8u))) != 0u; }
static void flip(uint8_t *v,uint32_t b) { v[b/8u] ^= (uint8_t)(1u << (b%8u)); }
static bool parity8(uint8_t v) { v^=(uint8_t)(v>>4);v^=(uint8_t)(v>>2);v^=(uint8_t)(v>>1);return (v&1u)!=0u; }
bool mobius_forward(uint8_t output[32],const uint8_t input[32])
{
 uint32_t cut,step; size_t j; uint8_t in[32];
 if(output==NULL||input==NULL) return false;
 for(j=0u;j<32u;++j) in[j]=input[j];
 for(j=0u;j<32u;++j) output[j]=0u;
 for(cut=0u;cut<256u;++cut) if(get(in,cut)) {
  uint32_t x=cut&31u,y=0u,slope=cut>>5;
  for(step=0u;step<64u;++step) {
   uint8_t value=nibble(surface[y][x]);
   if((step&1u)==0u) output[step/2u]^=(uint8_t)(value<<4); else output[step/2u]^=value;
   { uint32_t ny=(y+slope+(x&1u))&7u,nx=x+1u; if(nx>=32u){nx=0u;ny=7u-ny;}x=nx;y=ny; }
  }
 }
 return true;
}
bool mobius_inverse(uint8_t output[32],const uint8_t input[32])
{
 uint32_t i; size_t j; uint8_t in[32];
 if(output==NULL||input==NULL) return false;
 for(j=0u;j<32u;++j) in[j]=input[j];
 for(j=0u;j<32u;++j) output[j]=0u;
 for(i=0u;i<256u;++i) { size_t b; bool parity=false; for(b=0u;b<32u;++b) parity=parity^parity8((uint8_t)(mobius_inverse_rows_v1[i][b]&in[b])); if(parity) flip(output,i); }
 return true;
}
