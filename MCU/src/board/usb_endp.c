/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_endp.h"
#include "usb_desc.h"
#include "usb_istr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static HidEndp_t *pEndp;
static uint8_t *Receive_Buffer;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*!
 \brief
*/
void USB_SetHidEndp(HidEndp_t *endp)
{
    pEndp = endp;
    Receive_Buffer = endp->rx_buffer;
}

/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
    /* Read received data */
    USB_SIL_Read(EP1_OUT, Receive_Buffer);
    
    if (pEndp != NULL)
    {
        pEndp->rx_handler();
    }
    
    SetEPRxStatus(ENDP1, EP_RX_VALID);
}

///*******************************************************************************
//* Function Name  : EP1_OUT_Callback.
//* Description    : EP1 OUT Callback Routine.
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//void EP1_IN_Callback(void)
//{
//  /* Set the transfer complete token to inform upper layer that the current
//  transfer has been complete */
//  *xferComplete = 1;
//}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
