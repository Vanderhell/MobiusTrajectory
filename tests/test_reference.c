#include "mobius.h"
#include "reference.h"
#include <stdio.h>
#include <string.h>

static uint32_t next32(uint32_t *state)
{
    *state^=*state<<13;
    *state^=*state>>17;
    *state^=*state<<5;
    return *state;
}
int main(void)
{
    uint32_t case_index,i,state=UINT32_C(0x9E3779B9);
    for(case_index=0u;case_index<100000u;++case_index){
        uint8_t x[32],delta[32],xor_input[32],tx[32],td[32],txd[32],reference[32];
        uint8_t inverse_x[32],inverse_delta[32],inverse_xd[32],roundtrip[32];
        for(i=0u;i<32u;++i){x[i]=(uint8_t)next32(&state);delta[i]=(uint8_t)next32(&state);xor_input[i]=(uint8_t)(x[i]^delta[i]);}
        if(!mobius_forward(tx,x)){return 1;}
        reference_forward(reference,x);
        if(memcmp(tx,reference,32u)!=0){fprintf(stderr,"forward differential case=%u\n",case_index);return 1;}
        if(!mobius_inverse(roundtrip,tx)||memcmp(roundtrip,x,32u)!=0){fprintf(stderr,"inverse(forward(x)) case=%u\n",case_index);return 1;}
        if(!mobius_inverse(inverse_x,x)||!mobius_forward(roundtrip,inverse_x)||memcmp(roundtrip,x,32u)!=0){fprintf(stderr,"forward(inverse(x)) case=%u\n",case_index);return 1;}
        if(!mobius_forward(td,delta)||!mobius_forward(txd,xor_input)){return 1;}
        for(i=0u;i<32u;++i)td[i]^=tx[i];
        if(memcmp(td,txd,32u)!=0){fprintf(stderr,"forward linearity case=%u\n",case_index);return 1;}
        if(!mobius_inverse(inverse_delta,delta)||!mobius_inverse(inverse_xd,xor_input)){return 1;}
        for(i=0u;i<32u;++i)inverse_delta[i]^=inverse_x[i];
        if(memcmp(inverse_delta,inverse_xd,32u)!=0){fprintf(stderr,"inverse linearity case=%u\n",case_index);return 1;}
    }
    puts("100,000 differential cases, both inverse directions, forward and inverse linearity: PASS");
    return 0;
}
