#include "sc32f12xx.h"                  // Device header
#include "OLED_Font.h"
#include "sc32f1xxx_gpio.h"
#include "string.h"
#include "stdbool.h"

uint8_t Data_Null[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};


/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void _OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	uint8_t digits = 0;
	uint32_t temp = Number;
	
	if (Number == 0) {
		digits = 1;
	} else {
		uint32_t n = Number;
		while (n > 0 && digits < Length) {
			n /= 10;
			digits++;
		}
	}
	
	if (digits > Length) {
		digits = Length;
	}
	
	// Fill the number position with space
	for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, ' '); 
    }
	
	for (i = 0; i < digits; i++) {
		uint8_t digit = temp % 10;
		OLED_ShowChar(Line, Column + Length - 1 - i, digit + '0');
		temp /= 10;
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}



/* 小辅助：判断 UTF-8 一个字符所占字节数（处理 GBK(2) / UTF-8(3) 常见情况） */
/* 返回值：字符字节长度（1、2、3）; 如果无法识别默认返回1 */
static uint8_t _char_byte_len(const char *s)
{
    unsigned char c = (unsigned char)s[0];
    if ((c & 0x80) == 0x00) return 1;        // ASCII
    if ((c & 0xE0) == 0xC0) return 2;        // 2-byte UTF-8 (rare for CJK)
    if ((c & 0xF0) == 0xE0) return 3;        // 3-byte UTF-8 (CJK typical)
    if ((c & 0xF8) == 0xF0) return 4;        // 4-byte utf-8 (rare)
    // 可能为 GBK（高字节>=0x80），在 GBK 两字节表示情况下返回2
    if (c >= 0x80) return 2;
    return 1;
}

/* 统计给定 C 字符串按“显示字符”计数（UTF-8/GBK 兼容） */
static uint8_t _count_display_chars(const char *s)
{
    uint8_t cnt = 0;
    const char *p = s;
    size_t len = strlen(s);
    size_t i = 0;
    while (i < len) {
        uint8_t blen = _char_byte_len(p + i);
        i += blen;
        cnt++;
    }
    return cnt;
}

/* 复制单个“显示字符”(UTF-8/GBK)到目标 buffer，并返回字节长度 */
static uint8_t _copy_display_char(char *dest, const char *src)
{
    uint8_t blen = _char_byte_len(src);
    memcpy(dest, src, blen);
    dest[blen] = '\0';
    return blen;
}

/* -------- 替换/增强版：OLED_ShowChinese --------
   兼容 UTF-8(3 bytes) 和 GBK(2 bytes) 的单个或串联汉字显示。
   保持原接口：Line(1~4), Column(1~8, 每单位16像素), Chinese 字符串
*/
void OLED_ShowChinese(uint8_t Line, uint8_t Column, char *Chinese)
{
    uint8_t array_num = sizeof(OLED_CF16x16) / sizeof(OLED_CF16x16[0]);
    size_t len = strlen(Chinese);
    size_t i = 0;
    char SingleChinese[5]; // 支持最多4字节UTF-8 + 终止
    uint8_t pindex, j;

    while (i < len) {
        uint8_t blen = _char_byte_len(Chinese + i);
        if (blen > 4) blen = 4;
        memset(SingleChinese, 0, sizeof(SingleChinese));
        memcpy(SingleChinese, Chinese + i, blen);
        // 在字模表中查找
        for (pindex = 0; pindex < array_num; pindex++) {
            if (strcmp(OLED_CF16x16[pindex].Index, SingleChinese) == 0) {
                OLED_SetCursor((Line - 1) * 2, (Column - 1) * 16);
                for (j = 0; j < 16; j++) {
                    OLED_WriteData(OLED_CF16x16[pindex].Data[j]);
                }
                OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 16);
                for (j = 0; j < 16; j++) {
                    OLED_WriteData(OLED_CF16x16[pindex].Data[j + 16]);
                }
                break;
            }
        }
        if (pindex == array_num) {
            // 未找到，显示空（避免错位）
            OLED_SetCursor((Line - 1) * 2, (Column - 1) * 16);
            for (j = 0; j < 16; j++) OLED_WriteData(Data_Null[j]);
            OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 16);
            for (j = 0; j < 16; j++) OLED_WriteData(Data_Null[j + 16]);
        }
        // 下一个字
        Column++;
        i += blen;
    }
}

/* -------- 替换/增强版：OLED_ShowChinese2 --------
   功能：在长度为 Length（汉字个数单位）的区域显示汉字字符串，
   并把剩余位置清空（避免重影）。
   Line: 1~4, Column: 起始汉字列(1~8), Length: 区域汉字个数
*/
void OLED_ShowChinese2(uint8_t Line, uint8_t Column, uint8_t Length, char *Chinese)
{
    uint8_t used_cnt = _count_display_chars(Chinese); // 实际字符个数（UTF-8/GBK 兼容）

    // 先显示字符串（会按字符自增 Column）
    OLED_ShowChinese(Line, Column, Chinese);

    // 再清除剩余汉字位（填 0）
    uint8_t i, j;
    for (i = used_cnt; i < Length; i++) {
        uint8_t current_col_idx = Column + i;
        // 上半 16 字节
        OLED_SetCursor((Line - 1) * 2, (current_col_idx - 1) * 16);
        for (j = 0; j < 16; j++) OLED_WriteData(0x00);
        // 下半 16 字节
        OLED_SetCursor((Line - 1) * 2 + 1, (current_col_idx - 1) * 16);
        for (j = 0; j < 16; j++) OLED_WriteData(0x00);
    }
}

void OLED_ShowInt(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length) {
    char buf[20];
    uint8_t pos = 0;
	uint8_t i;
	
    // 处理 NaN / Inf 的保护（如果需要可拓展）
    // 处理负号
    bool neg = false;
    if (Number < 0.0f) {
        neg = true;
        Number = -Number;
    }

    // 清空显示区域（写空格），避免旧字符残留
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Line, Column + i, ' ');
    }

    char intbuf[12];
    uint8_t intlen = 0;
    if (Number == 0) {
        intbuf[intlen++] = '0';
    } else {
        uint32_t t = Number;
        while (t > 0 && intlen < sizeof(intbuf) - 1) {
            intbuf[intlen++] = '0' + (t % 10);
            t /= 10;
        }
    }
    // 将整数部分倒序拷贝到 buf
    if (neg) {
        buf[pos++] = '-';
    }
    for (i = 0; i < intlen; i++) {
        buf[pos++] = intbuf[intlen - 1 - i];
    }
    buf[pos] = '\0';

    // 右对齐放置字符串
    uint8_t slen = pos;
    if (slen >= Length) {
        // 字串过长，显示最右边的 Length 字符（截断左边）
        uint8_t start = slen - Length;
        for (i = 0; i < Length; i++) {
            OLED_ShowChar(Line, Column + i, buf[start + i]);
        }
    } else {
        // 右对齐：起始位置偏移 = Length - slen
        uint8_t offset = Length - slen;
        for (i = 0; i < slen; i++) {
            OLED_ShowChar(Line, Column + offset + i, buf[i]);
        }
    }

}

/* -------- 更稳健的 OLED_ShowFloat（替代原实现） --------
   说明：
   - 将浮点数格式化到临时 buffer（支持负号、小数点），
   - 然后按 Length 右对齐到指定 Column（ASCII 列，单位 8 像素），
   - 写之前把区域填充空格（防止重影）。
   参数同原接口：Line(1~4), Column(1~16 ASCII 单位), Length(总字符宽度), DecLength(小数位)
*/
void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number, uint8_t Length, uint8_t DecLength)
{
    char buf[20];
    uint8_t pos = 0;

    // 处理 NaN / Inf 的保护（如果需要可拓展）
    // 处理负号
    bool neg = false;
    if (Number < 0.0f) {
        neg = true;
        Number = -Number;
    }

    // 取整数、小数
    uint32_t IntPart = (uint32_t)Number;
    uint32_t pow10 = 1;
    uint8_t i;
    for (i = 0; i < DecLength; i++) pow10 *= 10;

    // 四舍五入处理小数部分
    uint32_t DecPart = 0;
    if (DecLength > 0) {
        float frac = Number - (float)IntPart;
        DecPart = (uint32_t)(frac * pow10 + 0.5f);
        if (DecPart >= pow10) { // 进位到整数
            IntPart += 1;
            DecPart = 0;
        }
    }

    // 构造整数部分字符串（先反向）
    char intbuf[12];
    uint8_t intlen = 0;
    if (IntPart == 0) {
        intbuf[intlen++] = '0';
    } else {
        uint32_t t = IntPart;
        while (t > 0 && intlen < sizeof(intbuf) - 1) {
            intbuf[intlen++] = '0' + (t % 10);
            t /= 10;
        }
    }
    // 将整数部分倒序拷贝到 buf
    if (neg) {
        buf[pos++] = '-';
    }
    for (i = 0; i < intlen; i++) {
        buf[pos++] = intbuf[intlen - 1 - i];
    }

    // 小数部分
    if (DecLength > 0) {
        buf[pos++] = '.';
        // 取 DecLength 位，注意要补前导零
        char decbuf[12];
        for (i = 0; i < DecLength; i++) {
            decbuf[DecLength - 1 - i] = '0' + (DecPart % 10);
            DecPart /= 10;
        }
        for (i = 0; i < DecLength; i++) buf[pos++] = decbuf[i];
    }
    buf[pos] = '\0';

    // 清空显示区域（写空格），避免旧字符残留
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Line, Column + i, ' ');
    }

    // 右对齐放置字符串
    uint8_t slen = pos;
    if (slen >= Length) {
        // 字串过长，显示最右边的 Length 字符（截断左边）
        uint8_t start = slen - Length;
        for (i = 0; i < Length; i++) {
            OLED_ShowChar(Line, Column + i, buf[start + i]);
        }
    } else {
        // 右对齐：起始位置偏移 = Length - slen
        uint8_t offset = Length - slen;
        for (i = 0; i < slen; i++) {
            OLED_ShowChar(Line, Column + offset + i, buf[i]);
        }
    }

}

/* -------- 计算单位汉字列位置（避免覆盖数字/重影） --------
   参数：
     label_chinese_len : 标签占用的汉字个数（例如"皮重："为3）
     ascii_start_col   : ASCII起始列（8px 单位）
     ascii_len         : ASCII 区域长度（字符个数，8px 单位）
   返回：用于显示单位的最左侧汉字列（16px 单位，1..8）
   逻辑：计算 ASCII 区域结束的像素位置，然后放到下一个不重叠的 16px 汉字边界上。
*/
static uint8_t _calc_unit_chinese_col(uint8_t ascii_start_col, uint8_t ascii_len)
{
    uint16_t ascii_start_px = (ascii_start_col - 1) * 8;            // px
    uint16_t ascii_end_px = ascii_start_px + ascii_len * 8;        // 结束像素（右侧）
    // 找到第一个 (c-1)*16 >= ascii_end_px 的 c
    uint8_t c = (ascii_end_px + 15) / 16 + 1; // 向上取整 +1 因为 Column 基于 1
    if (c > 8) c = 8; // 限制在 1..8
    if (c < 1) c = 1;
    return c;
}


/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void _OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	_OLED_Clear();				//OLED清屏
}