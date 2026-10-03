#include "mobius.h"
#include "reference.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
 uint32_t i;
 for(i=0u;i<256u;++i){uint8_t x[32]={0},a[32],b[32],inv[32];x[i/8u]=(uint8_t)(1u<<(i%8u));if(!mobius_forward(a,x)){return 1;}reference_forward(b,x);if(memcmp(a,b,32u)!=0||!mobius_inverse(inv,a)||memcmp(inv,x,32u)!=0){fprintf(stderr,"forward basis/inverse bit=%u\n",i);return 1;}if(!mobius_inverse(a,x)||!mobius_forward(inv,a)||memcmp(inv,x,32u)!=0){fprintf(stderr,"inverse basis/forward bit=%u\n",i);return 1;} }
 puts("rank=256; 256 basis vectors match reference and both inverse round trips: PASS");return 0;
}
