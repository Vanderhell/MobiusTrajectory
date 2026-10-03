#include "mobius.h"
#include "reference.h"
#include <stdio.h>
#include <string.h>

static bool bit(const uint8_t *value,uint32_t index)
{
    return (value[index/8u]&(uint8_t)(1u<<(index%8u)))!=0u;
}
static void set(uint8_t *value,uint32_t index)
{
    value[index/8u]|=(uint8_t)(1u<<(index%8u));
}
int main(void)
{
    uint8_t matrix[256][64];uint32_t row,col,rank=0u;
    for(row=0u;row<256u;++row){uint32_t k;for(k=0u;k<64u;++k)matrix[row][k]=0u;set(matrix[row],256u+row);}
    for(col=0u;col<256u;++col){uint8_t input[32]={0},output[32];input[col/8u]=(uint8_t)(1u<<(col%8u));reference_forward(output,input);for(row=0u;row<256u;++row)if(bit(output,row))set(matrix[row],col);}
    for(col=0u;col<256u;++col){uint32_t pivot=rank,k;while(pivot<256u&&!bit(matrix[pivot],col))++pivot;if(pivot==256u)continue;if(pivot!=rank)for(k=0u;k<64u;++k){uint8_t t=matrix[rank][k];matrix[rank][k]=matrix[pivot][k];matrix[pivot][k]=t;}for(row=0u;row<256u;++row)if(row!=rank&&bit(matrix[row],col))for(k=0u;k<64u;++k)matrix[row][k]^=matrix[rank][k];++rank;}
    if(rank!=256u){fprintf(stderr,"reference rank=%u expected=256\n",rank);return 1;}
    for(col=0u;col<256u;++col){uint8_t input[32]={0},expected[32]={0},actual[32],forward[32];input[col/8u]=(uint8_t)(1u<<(col%8u));for(row=0u;row<256u;++row)if(bit(matrix[row],256u+col))set(expected,row);if(!mobius_inverse(actual,input)||memcmp(actual,expected,32u)!=0||!mobius_forward(forward,actual)||memcmp(forward,input,32u)!=0){fprintf(stderr,"independent inverse column=%u rank=%u\n",col,rank);return 1;}}
    puts("independent trajectory matrix rank=256; inverse matches all 256 reference columns: PASS");return 0;
}
