/* $Id$ */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// STM32F42XX LCD-TFT Controller (LTDC)

#include "hardware.h"

#include "display.h"
#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints
#include "spifuncs.h"

#if CPUSTYLE_STM32F && LCDMODE_LTDC

//#define GCR_MASK                     ((uint32_t)0x0FFE888F)  /* LTDC GCR Mask */
/** @defgroup LTDC_Pixelformat 
  * @{
  */
#define LTDC_Pixelformat_ARGB8888                  ((uint32_t)0x00000000)
#define LTDC_Pixelformat_RGB888                    ((uint32_t)0x00000001)
#define LTDC_Pixelformat_RGB565                    ((uint32_t)0x00000002)
#define LTDC_Pixelformat_ARGB1555                  ((uint32_t)0x00000003)
#define LTDC_Pixelformat_ARGB4444                  ((uint32_t)0x00000004)
#define LTDC_Pixelformat_L8                        ((uint32_t)0x00000005)
#define LTDC_Pixelformat_AL44                      ((uint32_t)0x00000006)
#define LTDC_Pixelformat_AL88                      ((uint32_t)0x00000007)
/** @defgroup LTDC_BlendingFactor1 
  * @{
  */

#define LTDC_BlendingFactor1_CA                       ((uint32_t)0x00000400)
#define LTDC_BlendingFactor1_PAxCA                    ((uint32_t)0x00000600)

/**
  * @}
  */
      
/** @defgroup LTDC_BlendingFactor2
  * @{
  */

#define LTDC_BlendingFactor2_CA                       ((uint32_t)0x00000005)
#define LTDC_BlendingFactor2_PAxCA                    ((uint32_t)0x00000007)

/**
  * @}
  */

/** @defgroup LTDC_Reload 
  * @{
  */
#define LTDC_IMReload                     LTDC_SRCR_IMR                         /*!< Immediately Reload. */
#define LTDC_VBReload                     LTDC_SRCR_VBR                         /*!< Vertical Blanking Reload. */

/**
  * @}
  */
  
/** @defgroup LTDC_HSPolarity 
  * @{
  */
#define LTDC_HSPolarity_AL                0                /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPolarity_AH                LTDC_GCR_HSPOL                        /*!< Horizontal Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_VSPolarity 
  * @{
  */
#define LTDC_VSPolarity_AL                0                /*!< Vertical Synchronization is active low. */
#define LTDC_VSPolarity_AH                LTDC_GCR_VSPOL                        /*!< Vertical Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_DEPolarity 
  * @{
  */
// 0: Not Data Enable polarity is active low
// 1: Not Data Enable polarity is active high 
#define LTDC_DEPolarity_AL                0                /*!< Data Enable, is active low. */
#define LTDC_DEPolarity_AH                LTDC_GCR_DEPOL                        /*!< Data Enable, is active high. */

/**
  * @}
  */

/** @defgroup LTDC_PCPolarity 
  * @{
  */
#define LTDC_PCPolarity_IPC               0                /*!< input pixel clock. */
#define LTDC_PCPolarity_IIPC              LTDC_GCR_PCPOL                        /*!< inverted input pixel clock. */


/* Exported types ------------------------------------------------------------*/
 
/** 
  * @brief  LTDC Init structure definition  
  */

typedef struct
{
  uint32_t LTDC_HSPolarity;                 /*!< configures the horizontal synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_HSPolarity */

  uint32_t LTDC_VSPolarity;                 /*!< configures the vertical synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_VSPolarity */

  uint32_t LTDC_DEPolarity;                 /*!< configures the data enable polarity. This parameter can
                                                 be one of value of @ref LTDC_DEPolarity */

  uint32_t LTDC_PCPolarity;                 /*!< configures the pixel clock polarity. This parameter can
                                                 be one of value of @ref LTDC_PCPolarity */

  uint32_t LTDC_HorizontalSync;             /*!< configures the number of Horizontal synchronization 
                                                 width. This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalSync;               /*!< configures the number of Vertical synchronization 
                                                 heigh. This parameter must range from 0x000 to 0x7FF. */

  uint32_t LTDC_AccumulatedHBP;             /*!< configures the accumulated horizontal back porch width.
                                                 This parameter must range from LTDC_HorizontalSync to 0xFFF. */

  uint32_t LTDC_AccumulatedVBP;             /*!< configures the accumulated vertical back porch heigh.
                                                 This parameter must range from LTDC_VerticalSync to 0x7FF. */
            
  uint32_t LTDC_AccumulatedActiveW;         /*!< configures the accumulated active width. This parameter 
                                                 must range from LTDC_AccumulatedHBP to 0xFFF. */

  uint32_t LTDC_AccumulatedActiveH;         /*!< configures the accumulated active heigh. This parameter 
                                                 must range from LTDC_AccumulatedVBP to 0x7FF. */

  uint32_t LTDC_TotalWidth;                 /*!< configures the total width. This parameter 
                                                 must range from LTDC_AccumulatedActiveW to 0xFFF. */

  uint32_t LTDC_TotalHeigh;                 /*!< configures the total heigh. This parameter 
                                                 must range from LTDC_AccumulatedActiveH to 0x7FF. */
            
  uint32_t LTDC_BackgroundRedValue;         /*!< configures the background red value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_BackgroundGreenValue;       /*!< configures the background green value.
                                                 This parameter must range from 0x00 to 0xFF. */ 

   uint32_t LTDC_BackgroundBlueValue;       /*!< configures the background blue value.
                                                 This parameter must range from 0x00 to 0xFF. */
} LTDC_InitTypeDef;

/** 
  * @brief  LTDC Layer structure definition  
  */

typedef struct
{
  uint32_t LTDC_HorizontalStart;            /*!< Configures the Window Horizontal Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */
            
  uint32_t LTDC_HorizontalStop;             /*!< Configures the Window Horizontal Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_VerticalStart;              /*!< Configures the Window vertical Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalStop;               /*!< Configures the Window vaertical Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_PixelFormat;                /*!< Specifies the pixel format. This parameter can be 
                                                 one of value of @ref LTDC_Pixelformat */

  uint32_t LTDC_ConstantAlpha;              /*!< Specifies the constant alpha used for blending.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorBlue;           /*!< Configures the default blue value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorGreen;          /*!< Configures the default green value.
                                                 This parameter must range from 0x00 to 0xFF. */
            
  uint32_t LTDC_DefaultColorRed;            /*!< Configures the default red value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorAlpha;          /*!< Configures the default alpha value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_BlendingFactor_1;           /*!< Select the blending factor 1. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t LTDC_BlendingFactor_2;           /*!< Select the blending factor 2. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor2 */
            
  uint32_t LTDC_CFBStartAdress;             /*!< Configures the color frame buffer address */

  uint32_t LTDC_CFBLineLength;              /*!< Configures the color frame buffer line length. 
                                                 This parameter must range from 0x0000 to 0x1FFF. */

  uint32_t LTDC_CFBPitch;                   /*!< Configures the color frame buffer pitch in bytes.
                                                 This parameter must range from 0x0000 to 0x1FFF. */
                                                 
  uint32_t LTDC_CFBLineNumber;              /*!< Specifies the number of line in frame buffer. 
                                                 This parameter must range from 0x000 to 0x7FF. */
} LTDC_Layer_InitTypeDef;

static void 
LCD_AF_GPIOConfig(void)
{
	enum
	{
		GPIO_AF_LTDC = 14,  /* LCD-TFT Alternate Function mapping */
		GPIO_AF_LTDC9 = 9  /* LCD-TFT Alternate Function mapping */
	};
#if CTLSTYLE_V1D


	//arm_hardware_piof_outputs((1U << 10), 1 * (1U << 10));	// PF10 DE=constant high

	/* GPIOs Configuration */
	/*
	+------------------------+-----------------------+----------------------------+
	+                       LCD pins assignment                                   +
	+------------------------+-----------------------+----------------------------+
	|  LCD_TFT R2 <-> PC.10  |  LCD_TFT G2 <-> PA.06 |  LCD_TFT B2 <-> PD.06      |
	|  LCD_TFT R3 <-> PB.00! |  LCD_TFT G3 <-> PG.10!|  LCD_TFT B3 <-> PG.11      |
	|  LCD_TFT R4 <-> PA.11  |  LCD_TFT G4 <-> PB.10 |  LCD_TFT B4 <-> PG.12!     |
	|  LCD_TFT R5 <-> PA.12  |  LCD_TFT G5 <-> PB.11 |  LCD_TFT B5 <-> PA.03      |
	|  LCD_TFT R6 <-> PB.01! |  LCD_TFT G6 <-> PC.07 |  LCD_TFT B6 <-> PB.08      |
	|  LCD_TFT R7 <-> PG.06  |  LCD_TFT G7 <-> PD.03 |  LCD_TFT B7 <-> PB.09      |
	-------------------------------------------------------------------------------
			|  LCD_TFT HSYNC <-> PC.06  | LCDTFT VSYNC <->  PA.04 |
			|  LCD_TFT CLK   <-> PG.07  | LCD_TFT DE   <->  PF.10 |
			-------------------------------------------------------

	*/
	// AF_14
	arm_hardware_pioa_altfn50((1U << 3) | (1U << 4) | (1U << 6) | (1U << 11) | (1U << 12), GPIO_AF_LTDC);
	arm_hardware_piob_altfn50((1U << 8) | (1U << 9) | (1U << 10) | (1U << 11), GPIO_AF_LTDC);
	arm_hardware_pioc_altfn50((1U << 6) | (1U << 7) | (1U << 12), GPIO_AF_LTDC);
	arm_hardware_piod_altfn50((1U << 3) | (1U << 6), GPIO_AF_LTDC);
	arm_hardware_piof_altfn50((1U << 10), GPIO_AF_LTDC);
	arm_hardware_piog_altfn50((1U << 6) | (1U << 7) | (1U << 11), GPIO_AF_LTDC);
	// AF_9
	arm_hardware_piob_altfn50((1U << 0) | (1U << 1), GPIO_AF_LTDC9);
	arm_hardware_piog_altfn50((1U << 10) | (1U << 12), GPIO_AF_LTDC9);

#elif CTLSTYLE_V3D

	arm_hardware_pioi_outputs((1U << 12), 1 * (1U << 12));	// PI12 DISP=constant high

	// COMTROL SIGNALS
	arm_hardware_pioi_altfn50((1U << 9), GPIO_AF_LTDC);		// VSYNC
	arm_hardware_pioi_altfn50((1U << 10), GPIO_AF_LTDC);	// HSYNC
	arm_hardware_pioi_altfn50((1U << 14), GPIO_AF_LTDC);	// CLK
	//arm_hardware_piok_outputs((1U << 7), 1 * (1U << 7));		// DE = constant 1
	arm_hardware_piok_altfn50((1U << 7), GPIO_AF_LTDC);		// DE

	// REG
	arm_hardware_pioi_altfn50((1U << 15), GPIO_AF_LTDC);	// R0
	arm_hardware_pioj_altfn50((1U << 0), GPIO_AF_LTDC);		// R1
	arm_hardware_pioj_altfn50((1U << 1), GPIO_AF_LTDC);		// R2
	arm_hardware_pioj_altfn50((1U << 2), GPIO_AF_LTDC);		// R3
	arm_hardware_pioj_altfn50((1U << 3), GPIO_AF_LTDC);		// R4
	arm_hardware_pioj_altfn50((1U << 4), GPIO_AF_LTDC);		// R5
	arm_hardware_pioj_altfn50((1U << 5), GPIO_AF_LTDC);		// R6
	arm_hardware_pioj_altfn50((1U << 6), GPIO_AF_LTDC);		// R7

	// GREEN
	arm_hardware_pioj_altfn50((1U << 7), GPIO_AF_LTDC);		// G0
	arm_hardware_pioj_altfn50((1U << 8), GPIO_AF_LTDC);		// G1
	arm_hardware_pioj_altfn50((1U << 9), GPIO_AF_LTDC);		// G2
	arm_hardware_pioj_altfn50((1U << 10), GPIO_AF_LTDC);	// G3
	arm_hardware_pioj_altfn50((1U << 11), GPIO_AF_LTDC);	// G4
	arm_hardware_piok_altfn50((1U << 0), GPIO_AF_LTDC);		// G5
	arm_hardware_piok_altfn50((1U << 1), GPIO_AF_LTDC);		// G6
	arm_hardware_piok_altfn50((1U << 2), GPIO_AF_LTDC);		// G7

	// BLUE
	arm_hardware_pioe_altfn50((1U << 4), GPIO_AF_LTDC);		// B0
	arm_hardware_pioj_altfn50((1U << 13), GPIO_AF_LTDC);	// B1
	arm_hardware_pioj_altfn50((1U << 14), GPIO_AF_LTDC);	// B2
	arm_hardware_pioj_altfn50((1U << 15), GPIO_AF_LTDC);	// B3
	arm_hardware_piog_altfn50((1U << 12), GPIO_AF_LTDC9);	// B4
	arm_hardware_piok_altfn50((1U << 4), GPIO_AF_LTDC);		// B5
	arm_hardware_piok_altfn50((1U << 5), GPIO_AF_LTDC);		// B6
	arm_hardware_piok_altfn50((1U << 6), GPIO_AF_LTDC);		// B7

#else

	#error Wrong CTLSTYLE_xxx for use LCDMODE_LTDC

#endif
}

#if LCDMODE_LTDC_L24

	#define SCALE_H 3

// ������ ������� ����������� ������ ���������� �������� 
static void
fillLUT_L24(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
		uint_fast8_t r = color, g = color, b = color;
		
		/* ������ �������� � �������� ������� */
		LTDC_Layerx->CLUTWR = 
			((color << 24) & LTDC_LxCLUTWR_CLUTADD) |
			((r << 16) & LTDC_LxCLUTWR_RED) |
			((g << 8) & LTDC_LxCLUTWR_GREEN) |
			((b << 0) & LTDC_LxCLUTWR_BLUE);
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}

#elif LCDMODE_LTDC_L8
	#define SCALE_H 1

static void
fillLUT_L8(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
#define XRGB(zr,zg,zb) do { r = (zr), g = (zg), b = (zb); } while (0)
		uint_fast8_t r, g, b;

		switch (color)
		{
		case TFTRGB(0, 0, 0)			/*COLOR_BLACK*/:		XRGB(0, 0, 0);			break;	// 0x00 ������
		case TFTRGB(255, 0, 0)			/*COLOR_RED*/:			XRGB(255, 0, 0);		break; 	// 0xE0 �������
		case TFTRGB(0, 255, 0)			/*COLOR_GREEN*/:		XRGB(0, 255, 0);		break; 	// 0x1C �������
		case TFTRGB(0, 0, 255)			/*COLOR_BLUE*/:			XRGB(0, 0, 255);		break; 	// 0x03 �����
		case TFTRGB(128, 0, 0)			/*COLOR_DARKRED*/:		XRGB(128, 0, 0);		break; 	// 
		case TFTRGB(0, 128, 0)			/*COLOR_DARKGREEN*/:	XRGB(0, 128, 0);		break; 	// 
		case TFTRGB(0, 0, 128)			/*COLOR_DARKBLUE*/:		XRGB(0, 0, 128);		break; 	// 
		case TFTRGB(255, 255, 0)		/*COLOR_YELLOW*/:		XRGB(255, 255, 0);		break; 	// 0xFC ������
		case TFTRGB(255, 0, 255)		/*COLOR_MAGENTA*/:		XRGB(255, 0, 255);		break; 	// 0x83 ���������
		case TFTRGB(0, 255, 255)		/*COLOR_CYAN*/:			XRGB(0, 255, 255);		break; 	// 0x1F �������
		case TFTRGB(255, 255, 255)		/*COLOR_WHITE*/:		XRGB(255, 255, 255);	break;  // 0xff	�����
		case TFTRGB(128, 128, 128)		/*COLOR_GRAY*/:			XRGB(128, 128, 128);	break; 	// �����
		case TFTRGB(0xa5, 0x2a, 0x2a)	/*COLOR_BROWN*/:		XRGB(0xa5, 0x2a, 0x2a);	break; 	// 0x64 ����������
		case TFTRGB(0xff, 0xd7, 0x00)	/*COLOR_GOLD*/:			XRGB(0xff, 0xd7, 0x00);	break; 	// 0xF4 ������
		case TFTRGB(0xd1, 0xe2, 0x31)	/*COLOR_PEAR*/:			XRGB(0xd1, 0xe2, 0x31);	break; 	// 0xDC ��������
#undef XRGB
		default:
			r = ((color & 0xe0) << 0) | ((color & 0x80) ? 0x1f : 0);	// red
			g = ((color & 0x1c) << 3) | ((color & 0x10) ? 0x1f : 0);	// green
			b = ((color & 0x03) << 6) | ((color & 0x02) ? 0x3f : 0);	// blue
			break;
		}
		/* ������ �������� � �������� ������� */
		LTDC_Layerx->CLUTWR = 
			((color << 24) & LTDC_LxCLUTWR_CLUTADD) |
			((r << 16) & LTDC_LxCLUTWR_RED) |
			((g << 8) & LTDC_LxCLUTWR_GREEN) |
			((b << 0) & LTDC_LxCLUTWR_BLUE);
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}
#else
	#define SCALE_H 1

#endif /* LCDMODE_LTDC_L8 */


/**
  * @brief  Initializes the LTDC Layer according to the specified parameters
  *         in the LTDC_LayerStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_layerx: Select the layer to be configured, this parameter can be 
  *         one of the following values: LTDC_Layer1, LTDC_Layer2    
  * @param  LTDC_LayerStruct: pointer to a LTDC_LayerTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void 
LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, const LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct)
{

  uint32_t whsppos = 0;
  uint32_t wvsppos = 0;
  uint32_t dcgreen = 0;
  uint32_t dcred = 0;
  uint32_t dcalpha = 0;
  uint32_t cfbp = 0;

  /* Configures the horizontal start and stop position */
  whsppos = LTDC_Layer_InitStruct->LTDC_HorizontalStop << 16;
  LTDC_Layerx->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  LTDC_Layerx->WHPCR |= (LTDC_Layer_InitStruct->LTDC_HorizontalStart | whsppos);

  /* Configures the vertical start and stop position */
  wvsppos = LTDC_Layer_InitStruct->LTDC_VerticalStop << 16;
  LTDC_Layerx->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  LTDC_Layerx->WVPCR |= (LTDC_Layer_InitStruct->LTDC_VerticalStart | wvsppos);

  /* Specifies the pixel format */
  LTDC_Layerx->PFCR &= ~(LTDC_LxPFCR_PF);
  LTDC_Layerx->PFCR |= (LTDC_Layer_InitStruct->LTDC_PixelFormat);

  /* Configures the default color values */
  dcgreen = (LTDC_Layer_InitStruct->LTDC_DefaultColorGreen << 8);
  dcred = (LTDC_Layer_InitStruct->LTDC_DefaultColorRed << 16);
  dcalpha = (LTDC_Layer_InitStruct->LTDC_DefaultColorAlpha << 24);
  LTDC_Layerx->DCCR &=  ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
  LTDC_Layerx->DCCR |= (LTDC_Layer_InitStruct->LTDC_DefaultColorBlue | dcgreen | \
                        dcred | dcalpha);

  /* Specifies the constant alpha value */      
  LTDC_Layerx->CACR &= ~(LTDC_LxCACR_CONSTA);
  LTDC_Layerx->CACR |= (LTDC_Layer_InitStruct->LTDC_ConstantAlpha);

  /* Specifies the blending factors */
  LTDC_Layerx->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
  LTDC_Layerx->BFCR |= (LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 | LTDC_Layer_InitStruct->LTDC_BlendingFactor_2);

  /* Configures the color frame buffer start address */
  LTDC_Layerx->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
  LTDC_Layerx->CFBAR |= (LTDC_Layer_InitStruct->LTDC_CFBStartAdress);

  /* Configures the color frame buffer pitch in byte */
  cfbp = (LTDC_Layer_InitStruct->LTDC_CFBPitch << 16);
  LTDC_Layerx->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
  LTDC_Layerx->CFBLR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineLength | cfbp);

  /* Configures the frame buffer line number */
  LTDC_Layerx->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
  LTDC_Layerx->CFBLNR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineNumber);

}
/**
  * @brief  Initializes the LTDC peripheral according to the specified parameters
  *         in the LTDC_InitStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_InitStruct: pointer to a LTDC_InitTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct)
{
  uint32_t horizontalsync = 0;
  uint32_t accumulatedHBP = 0;
  uint32_t accumulatedactiveW = 0;
  uint32_t totalwidth = 0;
  uint32_t backgreen = 0;
  uint32_t backred = 0;

  /* Sets Synchronization size */
  LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
  horizontalsync = (LTDC_InitStruct->LTDC_HorizontalSync << LTDC_SSCR_HSW_Pos);
  LTDC->SSCR |= (horizontalsync | LTDC_InitStruct->LTDC_VerticalSync);

  /* Sets Accumulated Back porch */
  LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
  accumulatedHBP = (LTDC_InitStruct->LTDC_AccumulatedHBP << LTDC_BPCR_AHBP_Pos);
  LTDC->BPCR |= (accumulatedHBP | LTDC_InitStruct->LTDC_AccumulatedVBP);

  /* Sets Accumulated Active Width */
  LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
  accumulatedactiveW = (LTDC_InitStruct->LTDC_AccumulatedActiveW << LTDC_AWCR_AAW_Pos);
  LTDC->AWCR |= (accumulatedactiveW | LTDC_InitStruct->LTDC_AccumulatedActiveH);

  /* Sets Total Width */
  LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
  totalwidth = (LTDC_InitStruct->LTDC_TotalWidth << LTDC_TWCR_TOTALW_Pos);
  LTDC->TWCR |= (totalwidth | LTDC_InitStruct->LTDC_TotalHeigh);

  //LTDC->GCR &= (uint32_t)GCR_MASK;
  LTDC->GCR = 0;
  LTDC->GCR |=  (uint32_t)(LTDC_InitStruct->LTDC_HSPolarity | LTDC_InitStruct->LTDC_VSPolarity |
                           LTDC_InitStruct->LTDC_DEPolarity | LTDC_InitStruct->LTDC_PCPolarity);

  /* sets the background color value */
  backgreen = (LTDC_InitStruct->LTDC_BackgroundGreenValue << 8);
  backred = (LTDC_InitStruct->LTDC_BackgroundRedValue << 16);

  LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  LTDC->BCCR |= (backred | backgreen | LTDC_InitStruct->LTDC_BackgroundBlueValue);

}

static void LCD_LayerInit(
	unsigned hs,	// same as AccumulatedHBP + 1
	unsigned vs		// same as LTDC_AccumulatedVBP + 1
	)
{
	const unsigned rowsize = sizeof framebuff [0];	// ������ ����� ������ � ������
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
	/* Windowing configuration */
	/* In this case all the active display area is used to display a picture then :
	Horizontal start = horizontal synchronization + Horizontal back porch = 30 
	Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
	Vertical start   = vertical synchronization + vertical back porch     = 4
	Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = hs;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (DIM_SECOND * SCALE_H + hs - 1); 
	LTDC_Layer_InitStruct.LTDC_VerticalStart = vs;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (DIM_FIRST + vs - 1);

	/* Pixel Format configuration*/
	#if LCDMODE_LTDC_L8
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_L8;
	#elif LCDMODE_LTDC_L24
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_L8;
	#else /* LCDMODE_LTDC_L8 */
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	#endif /* LCDMODE_LTDC_L8 */
	/* Alpha constant (255 totally opaque) */
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
	/* Default Color configuration (configure A,R,G,B component values) */          
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	/* Configure blending factors */       
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

	/* the length of one line of pixels in bytes + 3 then :
	Line Lenth = Active high width x number of bytes per pixel + 3 
	Active high width         = DIM_SECOND 
	number of bytes per pixel = 2    (pixel_format : RGB565) 
	number of bytes per pixel = 1    (pixel_format : L8) 
	*/
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = rowsize + 3; //((DIM_SECOND * 2) + 3);
	//LTDC_Layer1->CFBLR = ((rowsize << 16) & LTDC_LxCFBLR_CFBP) | (((rowsize + 3) << 0) & LTDC_LxCFBLR_CFBLL);
	/* the pitch is the increment from the start of one line of pixels to the 
	start of the next line in bytes, then :
	Pitch = Active high width x number of bytes per pixel */ 
	LTDC_Layer_InitStruct.LTDC_CFBPitch = rowsize; // (DIM_SECOND * 2);

	/* Configure the number of lines */  
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = DIM_FIRST;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (unsigned long) & framebuff;
	//LTDC_Layer1->CFBAR = (uint32_t) & framebuff;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	/* Enable foreground & background Layers */
	//LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	/* Enable LTDC_Layer by setting LEN bit */
	LTDC_Layer1->CR |= (uint32_t)LTDC_LxCR_LEN;

	/* LTDC configuration reload */  
}

void
arm_hardware_ltdc_initialize(void)
{
	const int rowsize = (sizeof framebuff [0]);	// ������ ����� ������ � ������


	/* Initialize the LCD */
	//LCD_Init();

	/* Enable the LTDC Clock */
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;	/* LTDC clock enable */
	__DSB();

	/* Enable the DMA2D Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;	/* DMA2D clock enable */
	__DSB();

	/* Configure the LCD Control pins */
	LCD_AF_GPIOConfig();  

	/* Enable Pixel Clock --------------------------------------------------------*/

	/* Configure PLLSAI prescalers for LCD */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/3 = 64 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64/8 = 8 Mhz */
	//RCC_PLLSAIConfig(192, 7, 3);
	//RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);

	/* Enable PLLSAI Clock */
	//RCC_PLLSAICmd(ENABLE);
	/* Wait for PLLSAI activation */
	//while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
	//{
	//}


	/* LTDC Initialization -------------------------------------------------------*/
	LTDC_InitTypeDef LTDC_InitStruct;

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - ����� STM32F746G-DISCO
	/** 
	  * @brief  RK043FN48H Size  
	  */    
	enum
	{

		WIDTH = 480,				/* LCD PIXEL WIDTH            */
		HEIGHT = 272,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 41,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 10,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 2,					/* Vertical front porch       */
	};
#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
	enum
	{

		WIDTH = 320 * 3,				/* LCD PIXEL WIDTH            */
		HEIGHT = 240,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 1,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 1,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 2,					/* Vertical front porch       */
	};
#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
	enum
	{

		WIDTH = 240,				/* LCD PIXEL WIDTH            */
		HEIGHT = 320,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 10,				/* Horizontal synchronization */
		HBP = 20,				/* Horizontal back porch      */
		HFP = 10,				/* Horizontal front porch     */

		VSYNC = 2,				/* Vertical synchronization   */
		VBP = 4,					/* Vertical back porch        */
		VFP = 4,					/* Vertical front porch       */
	};
#else
#endif

	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
	//LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
	//LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AH;     
	/* Initialize the data enable polarity as active low */ 
	//LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Timing configuration */
	/* Configure horizontal synchronization width */     
	LTDC_InitStruct.LTDC_HorizontalSync = (HSYNC - 1);
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = (HSYNC + HBP - 1);
	/* Configure accumulated active width */  
	LTDC_InitStruct.LTDC_AccumulatedActiveW = (WIDTH + HSYNC + HBP - 1);
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = (WIDTH + HSYNC + HBP + HFP - 1);

	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = (VSYNC - 1);
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = (VSYNC + VBP - 1);
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = (HEIGHT + VSYNC + VBP - 1);
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = (HEIGHT + 1 * VSYNC + VBP + VFP - 1);

	LTDC_Init(&LTDC_InitStruct);

	/* Configure R,G,B component values for LCD background color */                   
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 

	LTDC_Init(&LTDC_InitStruct);


	/* LTDC initialization end ---------------------------------------------------*/

	LCD_LayerInit(HSYNC + HBP, VSYNC + VBP);

	LTDC->SRCR = LTDC_SRCR_IMR;


	/* LCD Log initialization */
	//LCD_LOG_Init(); 

	LTDC_Layer1->CFBLR = 
		((rowsize << 16) & LTDC_LxCFBLR_CFBP) | 
		(((rowsize + 3) << 0) & LTDC_LxCFBLR_CFBLL) |
		0;
	LTDC_Layer1->CFBAR = (uint32_t) & framebuff;
	//LTDC_Layer2->CFBAR = (uint32_t) & framebuff;

	/* Enable the LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

#if LCDMODE_LTDC_L24
	fillLUT_L24(LTDC_Layer1);	// ������ ���������� ���� ������ �� ������ �� �����. �������� ������� - ����� ����� �� Reload
#elif LCDMODE_LTDC_L8
	fillLUT_L8(LTDC_Layer1);	// �������� ������� - ����� ����� �� Reload
#endif /* LCDMODE_LTDC_L8 */

	/* LTDC reload configuration */  
	LTDC->SRCR = LTDC_SRCR_IMR;
}

#endif /* CPUSTYLE_STM32F && LCDMODE_LTDC */
