#include "mobius.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "golden.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t cases; static uint32_t rng=UINT32_C(0x9E3779B9);
static uint32_t next32(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static void fill(uint8_t x[32]){unsigned i;for(i=0;i<32u;++i)x[i]=(uint8_t)next32();}
static void fail(const char*t,uint32_t k){printf("MOBIUS_HW_TEST_FAIL test=%s index=%" PRIu32 " cases=%" PRIu64 "\n",t,k,cases);abort();}
static void basis(void){unsigned i;for(i=0;i<256u;++i){uint8_t x[32]={0},a[32],b[32];x[i/8u]=(uint8_t)(1u<<(i%8u));if(!mobius_forward(a,x)||!mobius_inverse(b,a)||memcmp(b,x,32u)!=0)fail("basis_inverse_forward",i);if(!mobius_inverse(a,x)||!mobius_forward(b,a)||memcmp(b,x,32u)!=0)fail("basis_forward_inverse",i);++cases;}}
static void patterns(void){uint8_t x[32],y[32],z[32];unsigned i;memset(x,0,sizeof x);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("zero",0);++cases;memset(x,0xFF,sizeof x);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("ones",0);++cases;memset(x,0xAA,sizeof x);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("AA",0);++cases;memset(x,0x55,sizeof x);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("55",0);++cases;memset(x,0,sizeof x);memset(x,0xFF,16u);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("low_half",0);++cases;memset(x,0,sizeof x);memset(x+16,0xFF,16u);if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("high_half",0);++cases;for(i=0;i<256u;++i){memset(x,0,sizeof x);x[i/8u]=(uint8_t)(1u<<(i%8u));if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("walking_one",i);memset(x,0xFF,sizeof x);x[i/8u]=(uint8_t)~(1u<<(i%8u));if(!mobius_forward(y,x)||!mobius_inverse(z,y)||memcmp(x,z,32u)!=0)fail("walking_zero",i);++cases;} }
static void golden(void){unsigned k;for(k=0;k<MOBIUS_GOLDEN_COUNT;++k){uint8_t out[32];if(!mobius_forward(out,mobius_golden_inputs[k])||memcmp(out,mobius_golden_outputs[k],32u)!=0)fail("golden_forward",k);++cases;}}
static void stress(void){uint32_t k;for(k=0;k<100000u;++k){uint8_t x[32],a[32],b[32];fill(x);if(!mobius_forward(a,x)||!mobius_inverse(b,a)||memcmp(b,x,32u)!=0)fail("inverse_forward",k);if(!mobius_inverse(a,x)||!mobius_forward(b,a)||memcmp(b,x,32u)!=0)fail("forward_inverse",k);cases+=2u;if((k&255u)==255u)vTaskDelay(1);if((k&16383u)==16383u)printf("MOBIUS_PROGRESS roundtrip=%" PRIu32 " cases=%" PRIu64 "\n",k+1u,cases);}}
static void linearity(void){uint32_t k;for(k=0;k<100000u;++k){uint8_t a[32],b[32],ax[32],fa[32],fb[32],fx[32],ia[32],ib[32],ix[32];unsigned i;fill(a);fill(b);for(i=0;i<32u;++i)ax[i]=(uint8_t)(a[i]^b[i]);if(!mobius_forward(fa,a)||!mobius_forward(fb,b)||!mobius_forward(fx,ax)||!mobius_inverse(ia,a)||!mobius_inverse(ib,b)||!mobius_inverse(ix,ax))fail("linearity_call",k);for(i=0;i<32u;++i){fa[i]^=fb[i];ia[i]^=ib[i];}if(memcmp(fa,fx,32u)!=0)fail("forward_linearity",k);if(memcmp(ia,ix,32u)!=0)fail("inverse_linearity",k);cases+=2u;if((k&255u)==255u)vTaskDelay(1);if((k&16383u)==16383u)printf("MOBIUS_PROGRESS linearity=%" PRIu32 " cases=%" PRIu64 "\n",k+1u,cases);}}
static void inplace(void){uint32_t k;for(k=0;k<10000u;++k){uint8_t x[32],expect[32],a[32],b[32];fill(x);if(!mobius_forward(expect,x))fail("inplace_forward_ref",k);memcpy(a,x,32u);if(!mobius_forward(a,a)||memcmp(a,expect,32u)!=0)fail("inplace_forward",k);if(!mobius_inverse(expect,x))fail("inplace_inverse_ref",k);memcpy(b,x,32u);if(!mobius_inverse(b,b)||memcmp(b,expect,32u)!=0)fail("inplace_inverse",k);cases+=2u;if((k&255u)==255u)vTaskDelay(1);}}
void app_main(void){unsigned run;
    printf("MOBIUS_HW_READY target=ESP32-S3 optimization=%s golden=%u\n",HW_OPT_LABEL,(unsigned)MOBIUS_GOLDEN_COUNT);
    for(run=0;run<HW_RUN_COUNT;++run){cases=0u;rng=UINT32_C(0x9E3779B9);patterns();basis();golden();stress();linearity();inplace();printf("MOBIUS_HW_TEST_PASS run=%u cases=%" PRIu64 " mismatches=0 free_heap=%u stack_high_water_words=%u\n",run+1u,cases,(unsigned)esp_get_free_heap_size(),(unsigned)uxTaskGetStackHighWaterMark(NULL));}
    vTaskDelay(pdMS_TO_TICKS(3000));esp_restart();
}
