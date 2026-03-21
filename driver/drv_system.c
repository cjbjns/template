#include "drv_system.h"

void drv_sys_init_pclk(uint8_t clk, uint8_t src, uint8_t div)
{
    if(clk == 1) //配置pclk1  --不支持选择时钟源
    {
        if(div > 0 && div < 16)
        {
            PCLK12_DIV &= ~0xF0;
            PCLK12_DIV |= (div - 1) << 4;
            PCLK_CR |= BIT(6);
        }
    }
    else if(clk == 2) //配置pclk2 --不支持选择时钟源
    {
        if(div > 0 && div < 16)
        {
            PCLK12_DIV &= ~0x0F;
            PCLK12_DIV |= (div - 1);
            PCLK_CR |= BIT(5);
        }
    }
    else if(clk == 3) //配置pclk3 --支持选择时钟源
    {
        if(div > 0 && src < 3)
        {
            PCLK_CR &= ~0x03;
            PCLK_CR |= (src + 1);

            PCLK3_DIV = div - 1;

            PCLK_CR |= BIT(4);
        }
    }
}

/*
1. PORKEY
2. 时钟树配置
3. 241的VCCIO必须外供
*/
void drv_system_init(void)
{
    /*init key*/
    POR_KEY0    = 0XCA; 
    POR_KEY1    = 0XCA;
    SOFT_RESET  = 0x0000;
    SCR_CFG     = 0x1010;

    /*imo x2, imo default 50MHz*/
    HCLK_CR = 0x0004; 
    HCLK_CR = 0x000C;

    SCR_HIS &= ~0x04; //default close shadow regs

    //固定自容高电平为2.0V，因为其与CSD_VCM_SEL的CC_VSEL有联动关系，而CC_VSEL又直接决定了CV补偿的V
    CSD_LDO_TRIM &= ~0x30;
    CSD_LDO_TRIM |= (0x02)<<4;

    CSD_LDO_CR = BIT(1)|BIT(0); //enable LDO

    //设置PCLK1的分频系数为10，若clk_sys=100MHz，则PCLK1=10M
    drv_sys_init_pclk(1, 0, 10);

    //设置PCLK2的分频系数为5，若clk_sys=100MHz，则PCLK1=20M
    drv_sys_init_pclk(2, 0, 5);

    //设置PCLK3的分频系数为1
    drv_sys_init_pclk(3, 2, 1);
}
