#include "mobius.h"
#include <stdio.h>
#include <string.h>

static int roundtrip(const char *name,const uint8_t input[32])
{
    uint8_t forward[32],back[32],inverse[32],forward_again[32];
    if(!mobius_forward(forward,input)||!mobius_inverse(back,forward)||memcmp(input,back,32u)!=0){fprintf(stderr,"inverse(forward(x)) pattern=%s\n",name);return 0;}
    if(!mobius_inverse(inverse,input)||!mobius_forward(forward_again,inverse)||memcmp(input,forward_again,32u)!=0){fprintf(stderr,"forward(inverse(x)) pattern=%s\n",name);return 0;}
    return 1;
}
int main(void)
{
    uint8_t x[32],y[32],base[32];uint32_t bit,i,k,state=UINT32_C(0x12345678);
    memset(x,0,32u);if(!roundtrip("zero",x))return 1;
    memset(x,UINT8_MAX,32u);if(!roundtrip("all-ones",x))return 1;
    memset(x,UINT8_C(0xAA),32u);if(!roundtrip("0xAA",x))return 1;
    memset(x,UINT8_C(0x55),32u);if(!roundtrip("0x55",x))return 1;
    memset(x,0,32u);if(!roundtrip("low-half-zero",x))return 1;
    memset(x,UINT8_MAX,16u);memset(x+16u,0,16u);if(!roundtrip("low-half-set",x))return 1;
    memset(x,0,16u);memset(x+16u,UINT8_MAX,16u);if(!roundtrip("high-half-set",x))return 1;
    for(i=0u;i<32u;++i){memset(x,0,32u);x[i]=UINT8_MAX;if(!roundtrip("single-byte",x))return 1;}
    for(bit=0u;bit<256u;++bit){memset(x,0,32u);x[bit/8u]=(uint8_t)(1u<<(bit%8u));if(!roundtrip("walking-one",x))return 1;memset(x,UINT8_MAX,32u);x[bit/8u]=(uint8_t)~(uint8_t)(1u<<(bit%8u));if(!roundtrip("walking-zero",x))return 1;}
    for(k=0u;k<10010u;++k){for(i=0u;i<32u;++i){if(k==0u)x[i]=0u;else if(k==1u)x[i]=UINT8_MAX;else if(k==2u)x[i]=(uint8_t)(i&1u?0x55u:0xAAu);else{state^=state<<13;state^=state>>17;state^=state<<5;x[i]=(uint8_t)state;}}if(!roundtrip("deterministic",x)){fprintf(stderr,"case=%u\n",k);return 1;}}
    if(mobius_forward(NULL,x)||mobius_forward(x,NULL)||mobius_inverse(y,NULL)||mobius_inverse(NULL,x))return 1;
    memset(x,UINT8_C(0xA5),32u);memcpy(base,x,32u);if(!mobius_forward(y,x)||!mobius_forward(x,x)||memcmp(x,y,32u)!=0||!mobius_inverse(x,x)||memcmp(x,base,32u)!=0)return 1;
    puts("structured, walking-bit, inverse, null, and in-place cases: PASS");return 0;
}
