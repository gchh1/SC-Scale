/**
 ******************************************************************************
 * @file    SC32f1xxx_OP.c
 * @author  SOC AE Team
 * @version V1.9.0
 * @date    2025-06-20
 * @brief   OP function module
 ******************************************************************************
 * @attention
 *
 *1.This software is supplied by SinOne Microelectronics Co.,Ltd. and is only 
 *intended for use with SinOne products. No other uses are authorized. This 
 *software is owned by SinOne Microelectronics Co.,Ltd. and is protected under 
 *all applicable laws, including copyright laws. 
 *2.The software which is for guidance only aims at providing customers with 
 *coding information regarding their products in order for them to save time. 
 *As a result, SinOne shall not be held liable for any direct, indirect or 
 *consequential damages with respect to any claims arising from the content of 
 *such software and/or the use made by customers of the coding information 
 *contained herein in connection with their products.
 *
 *  COPYRIGHT 2025 SinOne Microelectronics
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#if defined (SC32f12xx) 
#include "sc32f1xxx_op.h"
/** @defgroup OP_Exported_Functions_Group1 Configuration of the OP computation unit functions
 *  @brief   Configuration of the OP computation unit functions
 *
@verbatim
 ===============================================================================
                     ##### OP configuration functions #####
 ===============================================================================
@endverbatim
  * @{
  */

/**
 * @brief  DeInitialize the OP peripheral registers to their default reset values.
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP)
 *               - OP: OP can be select the OPx peripheral.
 *               - OP0: select the OP0 peripheral
 *               - OP1: select the OP1 peripheral
 *               - OP2: select the OP2 peripheral
 * @retval None
 */
void OP_DeInit ( OP_TypeDef* OPx )
{
#if defined (SC32f12xx) 
    uint32_t tmpreg;
    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );

    if ( OPx == OP )
    {
        /* Get the OPx OP_CON value */
        tmpreg = OPx->OP_CON;

        /* Set OPFS bit to OP_FreqSelect value */
        tmpreg &= 0x00000000;

        /* Write to OPx OP_CON */
        OPx->OP_CON = ( uint32_t ) tmpreg;
    }

#endif
}


/**
 * @brief  DeInitializes the OP peripheral
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP)
 *               - OP: OP can be select the OPx peripheral.
 *               - OP_0: select the OP0 peripheral
 *               - OP_1: select the OP1 peripheral
 *               - OP_2: select the OP2 peripheral
 * @param  OP_InitStruct[out]:Pointer to structure OP_InitTypeDef, to be initialized.
 * @retval None
 */
void OP_Init ( OP_TypeDef* OPx, OP_InitTypeDef* OP_InitStruct )
{
#if defined (SC32f12xx)
    uint32_t tmpreg;
    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );
    assert_param ( IS_OP_Output ( Output ) );
    assert_param ( IS_OP_Negative ( Negative ) );
    assert_param ( IS_OP_Posittive ( Posittive ) );
    assert_param ( IS_OP_PGAGain ( PGAGain ) );
    assert_param ( IS_OP_FDBResisrance ( FDBResisrance ) );
    assert_param ( IS_OP_ShortCircuit ( ShortCircuit ) );

    /*---------------------------- OPx OP_CON Configuration ------------------------*/
    /* Get the OPx OP_CON value */
    tmpreg = OPx->OP_CON;

    /* Clear OPFS bits */
    tmpreg &= ( uint32_t ) ~ ( OP_CON_OPOSEL | OP_CON_OPNSEL | OP_CON_OPPSEL
                               | OP_CON_PGAGAIN | OP_CON_FDBRSEL | OP_CON_PGAOFC );
    /* Set OPFS bit to OP_FreqSelect value */
    tmpreg |= ( uint32_t ) ( OP_InitStruct->OP_FDBResisrance | OP_InitStruct->OP_Negative | OP_InitStruct->OP_Output |
                             OP_InitStruct->OP_PGAGain | OP_InitStruct->OP_Posittive | OP_InitStruct->OP_ShortCircuit );

    /* Write to OPx OP_CON */
    OPx->OP_CON = tmpreg;

#endif
}

/**
 * @brief  Enables or disables the specified OP peripheral.
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP)
 *               - OP: OP can be select the OPx peripheral.
 *               - OP_0: select the OP0 peripheral
 *               - OP_1: select the OP1 peripheral
 *               - OP_2: select the OP2 peripheral
 * @param  NewState[in]: new state of the OPx peripheral.
 *                  - DISABLE:Function disable
 *                  - ENABLE:Function enable
 * @retval None
 */
void OP_Cmd ( OP_TypeDef* OPx, FunctionalState NewState )
{
#if defined (SC32f12xx)
    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );
    assert_param ( IS_FUNCTIONAL_STATE ( NewState ) );

    if ( NewState != DISABLE )
    {
        /* Enable the OP Function */
        OPx->OP_CON |= OP_CON_ENOP;
    }
    else
    {
        /* Disable the OP Function */
        OPx->OP_CON &= ( uint16_t ) ~OP_CON_ENOP;
    }

#endif
}

/**
  * @brief  Configure the trimming value of the OPAMP.
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP)
 *               - OP: OP can be select the OPx peripheral.
 *               - OP_0: select the OP0 peripheral
 *               - OP_1: select the OP1 peripheral
 *               - OP_2: select the OP2 peripheral
  * @param  OP_TrimValueH[in]: the trimming value(high).
  * @param  OP_TrimValueL[in]: the trimming value(low).
  * @retval None
  */
void OP_OffsetTrimConfig ( OP_TypeDef* OPx,  uint32_t OP_TrimValueH, uint32_t OP_TrimValueL )
{
#if defined (SC32f12xx)
    uint32_t tmpreg = 0;

    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );


    /*!< Get the OPAMPx_CSR register value */
    tmpreg = OPx->OP_CON;

    /*!< Clear the trimming bits */
    tmpreg &= ( uint32_t ) ~ ( ( OP_CON_TRIMOFFSETP | OP_CON_TRIMOFFSETN ) );

    /*!< Configure the new trimming value */
    tmpreg |= ( uint32_t ) ( ( OP_TrimValueL << OP_CON_TRIMOFFSETP_Pos ) | ( OP_TrimValueH << OP_CON_TRIMOFFSETN_Pos ) );

    /*!< Write to OPAMPx_CSR register */
    OPx->OP_CON = tmpreg;

#endif
}

/**
  * @brief  OP gain selection.
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP) 
 *               - OP: OP can be select the OPx peripheral.
 *               - OP_0: select the OP0 peripheral
 *               - OP_1: select the OP1 peripheral
 *               - OP_2: select the OP2 peripheral
  * @param  OPGain[in]: Gain multiple.
 *                SC32f12xx Selection range(OP_PGAGain_NonInvert8_Invert7,OP_PGAGain_NonInvert16_Invert15,OP_PGAGain_NonInvert32_Invert31,OP_PGAGain_NonInvert64_Invert63) 
  *                  - OP_PGAGain_NonInvert4_Invert3:The OP inverting input gain is 4/3 times
  *                  - OP_PGAGain_NonInvert8_Invert7:The OP inverting input gain is 8/7 times
  *                  - OP_PGAGain_NonInvert16_Invert15:The OP inverting input gain is 16/15 times
  *                  - OP_PGAGain_NonInvert32_Invert31:The OP in-phase input gain is 32/31 times
  *                  - OP_PGAGain_NonInvert64_Invert63:The OP in-phase input gain is 64/63 times
  * @retval None
  */
void OP_GainSelection ( OP_TypeDef* OPx, OP_PGAGain_TypeDef PGAGain )
{
#if defined (SC32f12xx)
    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );
    assert_param ( IS_OP_PGAGain ( PGAGain ) );


    OPx->OP_CON &= ~ ( OP_CON_PGAGAIN );
    OPx->OP_CON |= PGAGain;

#endif

}

/**
 * @}
 */
/**
  * @brief  OP Output port selection.
 * @param  OPx[out]:  can be select the OPx peripheral.
 *                SC32f12xx Selection range(OP)
 *               - OP: OP can be select the OPx peripheral.
 *               - OP_0: select the OP0 peripheral
 *               - OP_1: select the OP1 peripheral
 *               - OP_2: select the OP2 peripheral
  * @param  OPOutput[in]: Output port selection.
  *                  - OP_Output_OFF:The OP Output  OFF
  *                  - OP_Output_ON:The OP Output  ON
  * @retval None
  */
void OP_OutputSelection ( OP_TypeDef* OPx, OP_Output_TypeDef OPOutput )
{
#if defined (SC32f12xx)
    /* Check the parameters */
    assert_param ( IS_OP_ALL_PERIPH ( OPx ) );
    assert_param ( IS_OP_Output ( OPOutput ) );


    OPx->OP_CON &= ~ ( OP_CON_OPOSEL );
    OPx->OP_CON |= OPOutput;

#endif
}
#if defined (SC32f12xx)
/**
 * @brief  Check whether the OP check value is written correctly
 * @retval Set state:
 *               - SUCCESS
 *               - ERROR
 */
ErrorStatus OP_OffsetSet(OP_TypeDef* OPx)
{
		uint8_t OPx_Readoffset0,OPx_Readoffset1,OPx_Readoffset2,OPx_Readoffset3;
	  uint16_t OPx_offsetchecksum0,OPx_offsetchecksum1;
	  OPx_Readoffset0 =  *((uint8_t *)0X08C00000+0x480);
	  OPx_Readoffset1 =  *((uint8_t *)0X08C00000+0x481);
	  OPx_Readoffset2 =  *((uint8_t *)0X08C00000+0x482);
	  OPx_Readoffset3 =  *((uint8_t *)0X08C00000+0x483);
	  OPx_offsetchecksum0 = (*((uint8_t *)0X08C00000+0x480))+(*((uint8_t *)0X08C00000+0x481))+(*((uint8_t *)0X08C00000+0x482))+(*((uint8_t *)0X08C00000+0x483));
	  OPx_offsetchecksum1 = ((*((uint8_t *)0X08C00000+0x484))<<8)|(*((uint8_t *)0X08C00000+0x485));
    if (( OPx_Readoffset0==0x55 )&&( OPx_Readoffset2==0x55 )&&(OPx_offsetchecksum0==OPx_offsetchecksum1))
    {
	      uint32_t tmpreg = 0;
        /*!< Get the OPAMPx_CSR register value */
       tmpreg = OPx->OP_CON;

       /*!< Clear the trimming bits */
       tmpreg &= ( uint32_t ) ~ ( ( OP_CON_TRIMOFFSETN | OP_CON_TRIMOFFSETP ) );

       /*!< Configure the new trimming value */
       tmpreg |= ( uint32_t ) ( ( OPx_Readoffset3 << OP_CON_TRIMOFFSETN_Pos ) | ( OPx_Readoffset1 << OP_CON_TRIMOFFSETP_Pos ) );

       /*!< Write to OPAMPx_CSR register */
        OPx->OP_CON = tmpreg;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

#endif
#endif
/**
 * @}
 */
/**
 * @}
 */

/**
 * @}
 */

///************************ (C) COPYRIGHT SOC Microelectronics *****END OF FILE****/
