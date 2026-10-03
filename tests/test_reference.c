#include "mobius.h"
#include "reference.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
 uint32_t k,i,state=UINT32_C(0x9E3779B9); for(k=0u;k<20000u;++k){uint8_t x[32],delta[32],a[32],b[32],c[32];for(i=0u;i<32u;++i){state^=state<<13;state^=state>>17;state^=state<<5;x[i]=(uint8_t)state;delta[i]=(uint8_t)(i*29u);}if(!mobius_forward(a,x))return 1;reference_forward(b,x);if(memcmp(a,b,32u)!=0){fprintf(stderr,"reference case=%u\n",k);return 1;}for(i=0u;i<32u;++i)x[i]^=delta[i];if(!mobius_forward(c,x))return 1;for(i=0u;i<32u;++i)x[i]^=delta[i];if(!mobius_forward(b,delta))return 1;for(i=0u;i<32u;++i)b[i]^=a[i];if(memcmp(b,c,32u)!=0){fprintf(stderr,"linearity case=%u\n",k);return 1;}}
 puts("20,000 deterministic differential cases; GF(2) linearity: PASS");return 0;
}
