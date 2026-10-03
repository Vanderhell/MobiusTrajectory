#include "mobius.h"
#include "reference.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
 mobius_ctx_t c; uint32_t i; if(!mobius_init(&c)) {puts("rank=deficient");return 1;}
 for(i=0u;i<256u;++i){uint8_t x[32]={0},a[32],b[32];x[i/8u]=(uint8_t)(1u<<(i%8u));if(!mobius_forward(a,x)){return 1;}reference_forward(b,x);if(memcmp(a,b,32u)!=0){fprintf(stderr,"basis bit=%u\n",i);return 1;} }
 puts("canonical matrix rank=256; all 256 basis columns match independent reference: PASS");return 0;
}
