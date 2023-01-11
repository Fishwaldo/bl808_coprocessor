/**
 ******************************************************************************
 * @file    ipc_irq.c
 * @version V1.1
 * @date
 * @brief   Modified from the incomplete BL808 IPC driver
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
 * <h2><center>&copy; COPYRIGHT(c) 2023 Justin Hammond</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "ipc_irq.h"
#include "bflb_irq.h"
#include "ipc.h"

/** @addtogroup  BL606P_Peripheral_Driver
 *  @{
 */

/** @addtogroup  IPC
 *  @{
 */

/** @defgroup  IPC_Private_Macros
 *  @{
 */
#define IPC_LP_OFFSET_IN_M0 0
#define IPC_D0_OFFSET_IN_M0 16

#define IPC_M0_OFFSET_IN_LP 0
#define IPC_D0_OFFSET_IN_LP 16

#define IPC_M0_OFFSET_IN_D0 0
#define IPC_LP_OFFSET_IN_D0 16

/*@} end of group IPC_Private_Macros */

/** @defgroup  IPC_Private_Types
 *  @{
 */

/*@} end of group IPC_Private_Types */

/** @defgroup  IPC_Private_Variables
 *  @{
 */

ipcIntCallback *m0IpcIntCbfArra[GLB_CORE_ID_MAX - 1] = {NULL};
ipcIntCallback *lpIpcIntCbfArra[GLB_CORE_ID_MAX - 1] = {NULL};
ipcIntCallback *d0IpcIntCbfArra[GLB_CORE_ID_MAX - 1] = {NULL};

/*@} end of group IPC_Private_Variables */

/** @defgroup  IPC_Global_Variables
 *  @{
 */

/*@} end of group IPC_Global_Variables */

/** @defgroup  IPC_Private_Fun_Declaration
 *  @{
 */

void IPC_IRQ_LP_IRQHandler(int irq, void *);
void IPC_IRQ_D0_IRQHandler(int irq, void *);
void IPC_IRQ_M0_IRQHandler(int irq, void *);

BL_Err_Type IPC_IRQ_CPUx_Trigger_D0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout) __attribute__((warn_unused_result));
BL_Err_Type IPC_IRQ_CPUx_Trigger_LP(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout) __attribute__((warn_unused_result));
BL_Err_Type IPC_IRQ_CPUx_Trigger_M0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout) __attribute__((warn_unused_result));

/*@} end of group IPC_Private_Fun_Declaration */

/** @defgroup  IPC_Private_Functions
 *  @{
 */

/*@} end of group IPC_Private_Functions */

/** @defgroup  IPC_Public_Functions
 *  @{
 */
#if defined(CPU_M0) || defined(CPU_LP)
/****************************************************************************
* @brief  M0 IPC interrupt init
*
* @param  onLPTriggerCallBack: Callback when LP trigger
*
* @param  onD0TriggerCallBack: Callback when D0 trigger
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_Init(ipcIntCallback *onLPTriggerCallBack,
                 ipcIntCallback *onD0TriggerCallBack)
{
    m0IpcIntCbfArra[0] = onLPTriggerCallBack;
    m0IpcIntCbfArra[1] = onD0TriggerCallBack;

    IPC_IRQ_M0_Int_Unmask_By_Word(0xffffffff);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(IPC_M0_IRQn, IPC_IRQ_M0_IRQHandler);
    CPU_Interrupt_Enable(IPC_M0_IRQn);
#else
    bflb_irq_attach(IPC_M0_IRQn, IPC_IRQ_M0_IRQHandler, NULL);
    bflb_irq_enable(IPC_M0_IRQn);
#endif
}
#endif

/****************************************************************************
* @brief  M0 unmask IPC interrupt
*
* @param  src: M0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_Int_Unmask(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC0_BASE, IPC_CPU0_IPC_IUSR, tmpVal);
}

/**************************************************************************** 
* @brief  M0 unmask IPC interrupt by word
*
* @param  src: IPC interrupt source in word,every bit is interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_Int_Unmask_By_Word(uint32_t src)
{
    BL_WR_REG(IPC0_BASE, IPC_CPU0_IPC_IUSR, src);
}

/****************************************************************************
* @brief  M0 get IPC interrupt raw status
*
* @param  None
*
* @return IPC interrupt raw status
*
*******************************************************************************/
uint32_t IPC_IRQ_M0_Get_Int_Raw_Status(void)
{
    return BL_RD_REG(IPC0_BASE, IPC_CPU0_IPC_IRSRR);
}

/****************************************************************************
* @brief  M0 clear IPC interrupt
*
* @param  src: M0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_Clear_Int(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC0_BASE, IPC_CPU0_IPC_ICR, tmpVal);
}

/****************************************************************************
* @brief  M0 clear IPC interrupt by word
*
* @param  src: IPC interrupt source in word,every bit is interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_Clear_Int_By_Word(uint32_t src)
{
    BL_WR_REG(IPC0_BASE, IPC_CPU0_IPC_ICR, src);
}

/**************************************************************************** 
* @brief  CPUx trigger IPC interrupt to M0
*
* @param  src: IPC interrupt source
*
* @param  cpuxOffset: CPU interrupt offset
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_CPUx_Trigger_M0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_GRP_INT_SRC_Type(src));

    tmpVal = (1 << (src + cpuxOffset));
    BL_WR_REG(IPC0_BASE, IPC_CPU1_IPC_ISWR, tmpVal);

    uint64_t now = bflb_mtimer_get_time_ms();
    while (true) {
        if ((IPC_IRQ_M0_Get_Int_Raw_Status() & tmpVal) == false) {
            return SUCCESS;
        }
        if (bflb_mtimer_get_time_ms() - now > timeout) {
            return TIMEOUT;
        }
    }
    return ERROR;
}

/****************************************************************************
* @brief  LP trigger IPC interrupt to M0
*
* @param  src: LP IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_LP_Trigger_M0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_M0(src, IPC_LP_OFFSET_IN_M0, timeout);
}

/******************************************************************************
* @brief  D0 trigger IPC interrupt to M0
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_D0_Trigger_M0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_M0(src, IPC_D0_OFFSET_IN_M0, timeout);
}

#if defined(CPU_M0) || defined(CPU_LP)
/******************************************************************************
* @brief  LP IPC interrupt init
*
* @param  onM0TriggerCallBack: Callback when M0 trigger
*
* @param  onD0TriggerCallBack: Callback when D0 trigger
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_Init(ipcIntCallback *onM0TriggerCallBack,
                     ipcIntCallback *onD0TriggerCallBack)
{
    lpIpcIntCbfArra[0] = onM0TriggerCallBack;
    lpIpcIntCbfArra[1] = onD0TriggerCallBack;

    IPC_IRQ_LP_Int_Unmask_By_Word(0xffffffff);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(IPC_LP_IRQn, IPC_IRQ_LP_IRQHandler);
    CPU_Interrupt_Enable(IPC_LP_IRQn);
#else
    bflb_irq_attach(IPC_LP_IRQn, IPC_IRQ_LP_IRQHandler, NULL);
    bflb_irq_enable(IPC_LP_IRQn);
#endif
}
#endif

/******************************************************************************
* @brief  LP unmask IPC interrupt
*
* @param  src: LP IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_Int_Unmask(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC1_BASE, IPC_CPU0_IPC_IUSR, tmpVal);
}

/******************************************************************************
* @brief  LP unmask IPC interrupt by word
*
* @param  src: IPC interrupt source in word,every bit is interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_Int_Unmask_By_Word(uint32_t src)
{
    BL_WR_REG(IPC1_BASE, IPC_CPU0_IPC_IUSR, src);
}

/******************************************************************************
* @brief  LP get IPC interrupt raw status
*
* @param  None
*
* @return IPC interrupt raw status
*
*******************************************************************************/
uint32_t IPC_IRQ_LP_Get_Int_Raw_Status(void)
{
    return BL_RD_REG(IPC1_BASE, IPC_CPU0_IPC_IRSRR);
}

/******************************************************************************
* @brief  LP clear IPC interrupt
*
* @param  src: LP IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_Clear_Int(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC1_BASE, IPC_CPU0_IPC_ICR, tmpVal);
}

/******************************************************************************
* @brief  LP clear IPC interrupt by word
*
* @param  src: IPC interrupt source in word,every bit is interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_Clear_Int_By_Word(uint32_t src)
{
    BL_WR_REG(IPC1_BASE, IPC_CPU0_IPC_ICR, src);
}

/******************************************************************************
* @brief  CPUx trigger IPC interrupt to LP
*
* @param  src: IPC interrupt source
*
* @param  cpuxOffset: CPU interrupt offset
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_CPUx_Trigger_LP(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_GRP_INT_SRC_Type(src));

    tmpVal = (1 << (src + cpuxOffset));

    BL_WR_REG(IPC1_BASE, IPC_CPU1_IPC_ISWR, tmpVal);

    uint64_t now = bflb_mtimer_get_time_ms();
    while (true) {
        if ((IPC_IRQ_LP_Get_Int_Raw_Status() & tmpVal) == false) {
            return SUCCESS;
        }
        if (bflb_mtimer_get_time_ms() - now > timeout) {
            return TIMEOUT;
        }
    }
    return ERROR;
}

/******************************************************************************
* @brief  M0 trigger IPC interrupt to LP
*
* @param  src: M0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_M0_Trigger_LP(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_LP(src, IPC_M0_OFFSET_IN_LP, timeout);
}

/******************************************************************************
* @brief  D0 trigger IPC interrupt to LP
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_D0_Trigger_LP(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_LP(src, IPC_D0_OFFSET_IN_LP, timeout);
}

#if defined(CPU_D0) || defined(CPU_D1)
/******************************************************************************
* @brief  D0 IPC interrupt init
*
* @param  onM0TriggerCallBack: Callback when M0 trigger
*
* @param  onLPTriggerCallBack: Callback when LP trigger
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_Init(ipcIntCallback *onM0TriggerCallBack,
                     ipcIntCallback *onLPTriggerCallBack)
{
    d0IpcIntCbfArra[0] = onM0TriggerCallBack;
    d0IpcIntCbfArra[1] = onLPTriggerCallBack;

    IPC_IRQ_D0_Int_Unmask_By_Word(0xffffffff);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(IPC_D0_IRQn, IPC_IRQ_D0_IRQHandler);
    CPU_Interrupt_Enable(IPC_D0_IRQn);
#else
    bflb_irq_attach(IPC_D0_IRQn, IPC_IRQ_D0_IRQHandler, NULL);
    bflb_irq_enable(IPC_D0_IRQn);
#endif
}
#endif

/******************************************************************************
* @brief  D0 unmask IPC interrupt
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_Int_Unmask(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC2_BASE, IPC_CPU0_IPC_IUSR, tmpVal);
}

/******************************************************************************
* @brief  D0 unmask IPC interrupt by word
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_Int_Unmask_By_Word(uint32_t src)
{
    BL_WR_REG(IPC2_BASE, IPC_CPU0_IPC_IUSR, src);
}

/******************************************************************************
* @brief  D0 get IPC interrupt raw status
*
* @param  None
*
* @return IPC interrupt raw status
*
*******************************************************************************/
uint32_t IPC_IRQ_D0_Get_Int_Raw_Status(void)
{
    return BL_RD_REG(IPC2_BASE, IPC_CPU0_IPC_IRSRR);
}

/******************************************************************************
* @brief  D0 clear IPC interrupt
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_Clear_Int(IPC_IRQ_INT_SRC_Type src)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_INT_SRC_TYPE(src));

    tmpVal = (1 << src);

    BL_WR_REG(IPC2_BASE, IPC_CPU0_IPC_ICR, tmpVal);
}

/******************************************************************************
* @brief  D0 clear IPC interrupt by word
*
* @param  src: IPC interrupt source in word,every bit is interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_Clear_Int_By_Word(uint32_t src)
{
    BL_WR_REG(IPC2_BASE, IPC_CPU0_IPC_ICR, src);
}

/******************************************************************************
* @brief  CPUx trigger IPC interrupt to D0
*
* @param  src: IPC interrupt source
*
* @param  cpuxOffset: CPU interrupt offset
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_CPUx_Trigger_D0(IPC_IRQ_GRP_INT_SRC_Type src, uint8_t cpuxOffset, uint8_t timeout) 
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IPC_IRQ_GRP_INT_SRC_Type(src));

    tmpVal = (1 << (src + cpuxOffset));

    BL_WR_REG(IPC2_BASE, IPC_CPU1_IPC_ISWR, tmpVal);

    uint64_t now = bflb_mtimer_get_time_ms();
    while (true) {
        if ((IPC_IRQ_D0_Get_Int_Raw_Status() & tmpVal) == false) {
            return SUCCESS;
        }
        if (bflb_mtimer_get_time_ms() - now > timeout) {
            return TIMEOUT;
        }
    }
    return ERROR;
}

/******************************************************************************
* @brief  M0 trigger IPC interrupt to D0
*
* @param  src: M0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_M0_Trigger_D0(IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_D0(src, IPC_M0_OFFSET_IN_D0, timeout);
}

/******************************************************************************
* @brief  LP trigger IPC interrupt to D0
*
* @param  src: LP IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_LP_Trigger_D0(IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    return IPC_IRQ_CPUx_Trigger_D0(src, IPC_LP_OFFSET_IN_D0, timeout);
}

/******************************************************************************
* @brief  M0 trigger IPC interrupt to CPUx
*
* @param  tgtCPU: Target CPU
*
* @param  src: IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_M0_Trigger_CPUx(GLB_CORE_ID_Type tgtCPU, IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    switch (tgtCPU)
    {
    case GLB_CORE_ID_LP:
        return IPC_IRQ_M0_Trigger_LP(src, timeout);
        break;
    case GLB_CORE_ID_D0:
        return IPC_IRQ_M0_Trigger_D0(src, timeout);
        break;
    default:
        break;
    }
    return INVALID;
}

/******************************************************************************
* @brief  LP trigger IPC interrupt to CPUx
*
* @param  tgtCPU: Target CPU
*
* @param  src: IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_LP_Trigger_CPUx(GLB_CORE_ID_Type tgtCPU, IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    switch (tgtCPU)
    {
    case GLB_CORE_ID_M0:
        return IPC_IRQ_LP_Trigger_M0(src, timeout);
        break;
    case GLB_CORE_ID_D0:
        return IPC_IRQ_LP_Trigger_D0(src, timeout);
        break;
    default:
        break;
    }
    return INVALID;
}

/******************************************************************************
* @brief  D0 trigger IPC interrupt to CPUx
*
* @param  tgtCPU: Target CPU
*
* @param  src: IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_D0_Trigger_CPUx(GLB_CORE_ID_Type tgtCPU, IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    switch (tgtCPU)
    {
    case GLB_CORE_ID_M0:
        return IPC_IRQ_D0_Trigger_M0(src, timeout);
        break;
    case GLB_CORE_ID_LP:
        return IPC_IRQ_D0_Trigger_LP(src, timeout);
        break;
    default:
        break;
    }
    return INVALID;
}

/******************************************************************************
* @brief  D0 trigger IPC interrupt to D1
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
BL_Err_Type IPC_IRQ_Trigger_Target_CPU(GLB_CORE_ID_Type tgtCPU, IPC_IRQ_GRP_INT_SRC_Type src, uint32_t timeout)
{
    GLB_CORE_ID_Type localCPU = GLB_Get_Core_Type();

    switch (localCPU)
    {
    case GLB_CORE_ID_M0:
        return IPC_IRQ_M0_Trigger_CPUx(tgtCPU, src, timeout);
        break;
    case GLB_CORE_ID_LP:
        return IPC_IRQ_LP_Trigger_CPUx(tgtCPU, src, timeout);
        break;
    case GLB_CORE_ID_D0:
        return IPC_IRQ_D0_Trigger_CPUx(tgtCPU, src, timeout);
        break;
    default:
        break;
    }
    return INVALID;
}

/******************************************************************************
* @brief  D0 trigger IPC interrupt to D1
*
* @param  src: D0 IPC interrupt source
*
* @return None
*
*******************************************************************************/
void IPC_Common_Interrupt_Handler(uint32_t irqStatus, ipcIntCallback *callBack[GLB_CORE_ID_MAX - 1])
{
    uint32_t tmp;
    uint32_t grp = 0;
    for (grp = 0; grp < GLB_CORE_ID_MAX - 1; grp++)
    {
        tmp = (irqStatus >> (16 * grp)) & 0xffff;
        if (tmp != 0)
        {
            if (callBack[grp] != NULL)
            {
                callBack[grp](tmp);
            }
        }
    }
}

/******************************************************************************
* @brief  M0 IPC IRQ handler
*
* @param  None
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_M0_IRQHandler(int irq, void *param)
{
    uint32_t irqStatus;
    irqStatus = IPC_IRQ_M0_Get_Int_Raw_Status();
    IPC_Common_Interrupt_Handler(irqStatus, m0IpcIntCbfArra);
    IPC_IRQ_M0_Clear_Int_By_Word(irqStatus);
}

/******************************************************************************
* @brief  LP IPC IRQ handler
*
* @param  None
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_LP_IRQHandler(int irq, void *param)
{
    uint32_t irqStatus;
    irqStatus = IPC_IRQ_LP_Get_Int_Raw_Status();
    IPC_Common_Interrupt_Handler(irqStatus, lpIpcIntCbfArra);
    IPC_IRQ_LP_Clear_Int_By_Word(irqStatus);
}

/******************************************************************************
* @brief  D0 IPC IRQ handler
*
* @param  None
*
* @return None
*
*******************************************************************************/
void IPC_IRQ_D0_IRQHandler(int irq, void *param)
{
    uint32_t irqStatus;
    irqStatus = IPC_IRQ_D0_Get_Int_Raw_Status();
    IPC_Common_Interrupt_Handler(irqStatus, d0IpcIntCbfArra);
    IPC_IRQ_D0_Clear_Int_By_Word(irqStatus);
}

/*@} end of group IPC_Public_Functions */

/*@} end of group IPC */

/*@} end of group BL606P_Peripheral_Driver */
