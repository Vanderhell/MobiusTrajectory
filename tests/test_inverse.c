#include "mobius.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
 mobius_ctx_t c; uint8_t x[32],y[32],z[32]; uint32_t k,i,state=UINT32_C(0x12345678); if(!mobius_init(&c))return 1;
 for(k=0u;k<10010u;++k){for(i=0u;i<32u;++i){if(k==0u)x[i]=0u;else if(k==1u)x[i]=UINT8_MAX;else if(k==2u)x[i]=(uint8_t)(i&1u?0x55u:0xAAu);else {state^=state<<13;state^=state>>17;state^=state<<5;x[i]=(uint8_t)state;}}if(!mobius_forward(y,x)||!mobius_inverse(z,y,&c)||memcmp(x,z,32u)!=0||!mobius_inverse(y,x,&c)||!mobius_forward(z,y)||memcmp(x,z,32u)!=0){fprintf(stderr,"inverse case=%u\n",k);return 1;}}
 if(mobius_init(NULL)||mobius_forward(NULL,x)||mobius_inverse(z,x,NULL))return 1;
 for(i=0u;i<32u;++i) x[i]=UINT8_C(0xA5);
 if(!mobius_forward(y,x)||!mobius_forward(x,x)||memcmp(x,y,32u)!=0||!mobius_inverse(x,x,&c)) return 1;
 for(i=0u;i<32u;++i) if(x[i]!=UINT8_C(0xA5)) return 1;
 puts("zero, ones, alternating, and 10007 deterministic inverse cases: PASS");return 0;
}
