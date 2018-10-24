/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-05-29
***********************************************************************************
*/
//-------------------���ؿ⺯��------------------------------
#include "includes.h"
#include "GUI.h"
#include "EmWinXslApp.h"
#include "Module_SdFat.h"
#include "Language.h"
#include "uctsk_HCI.h"
#include "Bsp_Key.h"

#include "WM.h"
//-------------------�궨��----------------------------------
//���Ժ������࿪��
#define  EMWIN_XSL_APP_TEST_BASIC_TEXT    0
#define  EMWIN_XSL_APP_TEST_WRAP_TEXT     0
#define  EMWIN_XSL_APP_TEST_DISPDEC       0
#define  EMWIN_XSL_APP_TEST_BASIC_2D      1
#define  EMWIN_XSL_APP_TEST_BMP           0
#define  EMWIN_XSL_APP_TEST_BMP_TF        1
#define  EMWIN_XSL_APP_TEST_JPG_TF        0
#define  EMWIN_XSL_APP_TEST_GIF_TF        0
#define  EMWIN_XSL_APP_TEST_PNG_TF        0
//-------------------����------------------------------------
//���뻺�治��ʹ��CCM��ַ,��ΪSDIO����������DMA����,��CCM��֧��DMA
static char *pGuiBuffer=NULL;
//-------------------�ӿں���--------------------------------
/*
************************************************************************************
*   ����˵��: ��TF����ȡ���ݣ�������GUI_BMP_DrawEx()����
*   ��    �Σ�p             FIL��������
*             NumBytesReq   �����ȡ���ֽ���
*             ppData        ����ָ��
*             Off           ���Off = 1����ô�����´���ʵλ�ö�ȡ
*   �� �� ֵ: ���ض�ȡ���ֽ���
************************************************************************************
*/
static int EmWinXslApp_GetTfData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off)
{
    static int FileAddress = 0;
    UINT NumBytesRead;
    FIL *PicFile;

    PicFile = (FIL *)p;
    // ��⻺���С(�����ڴ���: ÿ�ι̶���ʾһ��,ÿ������4�ֽ�)
    if (NumBytesReq > LCD_GetXSize()*4)
    {
        NumBytesReq = LCD_GetXSize()*4;
    }
    // ���ö�ȡλ��
    if(Off == 1) FileAddress = 0;
    else FileAddress = Off;
    //
    ModuleSdFat_Res =f_lseek(PicFile, FileAddress);
    // ��ȡ���ݵ�����
    ModuleSdFat_Res = f_read(PicFile, pGuiBuffer, NumBytesReq, &NumBytesRead);
    // ��ָ��ppDataָ���ȡ�ĺ���
    *ppData = (const U8 *)pGuiBuffer;
    // ���ض�ȡ���ֽ���
    return NumBytesRead;
}

/*
*********************************************************************************************************
*   �� �� ��: _ShowBMPEx
*   ����˵��: ��ʾBMPͼƬ
*   ��    �Σ�sFilename Ҫ��ʾͼƬ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
/*
************************************************************************************
*   ����˵��: ��TF����ȡ���ݣ�������GUI_BMP_DrawEx()����
*   ��    �Σ�p             FIL��������
*             NumBytesReq   �����ȡ���ֽ���
*             ppData        ����ָ��
*             Off           ���Off = 1����ô�����´���ʵλ�ö�ȡ
*   �� �� ֵ: ���ض�ȡ���ֽ���
************************************************************************************
*/
static char *pStreamedBmpBeginAddr =  (char*)0x8075000;
static int EmWinXslApp_GetSpiFlashData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off)
{
    static int FileAddress = 0;
    UINT NumBytesRead;
    //FIL *PicFile;

    //PicFile = (FIL *)p;
    // ��⻺���С(�����ڴ���: ÿ�ι̶���ʾһ��,ÿ������4�ֽ�)
    if (NumBytesReq > LCD_GetXSize()*4)
    {
        NumBytesReq = LCD_GetXSize()*4;
    }
    // ���ö�ȡλ��
    if(Off == 1) FileAddress = 0;
    else FileAddress = Off;
    // ��ַƫ��
    // ModuleSdFat_Res = f_lseek(PicFile, FileAddress);
    // ��ȡ���ݵ�����
    // ModuleSdFat_Res = f_read(PicFile, pGuiBuffer, NumBytesReq, &NumBytesRead);
    memcpy(pGuiBuffer,pStreamedBmpBeginAddr+FileAddress,NumBytesReq);
    // ��ָ��ppDataָ���ȡ�ĺ���
    *ppData = (const U8 *)pGuiBuffer;
    NumBytesRead  =  NumBytesReq;
    // ���ض�ȡ���ֽ���
    return NumBytesRead;
}
uint8_t EmWinXslApp_ShowStreamedBMP(const char * sFilename,uint16_t xpos,uint16_t ypos)
{
    // ���뻺��(�����ڴ���: ÿ�ι̶���ʾһ��,ÿ������4�ֽ� ��320*4=1280B)
    pGuiBuffer = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    GUI_DrawStreamedBitmapExAuto(EmWinXslApp_GetSpiFlashData, NULL, xpos, ypos);
    //GUI_DrawStreamedBitmapAuto(pStreamedBmpBeginAddr,0,0);
    // �ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pGuiBuffer);
    //
    return OK;
}

/*
*********************************************************************************************************
*   �� �� ��: _ShowBMPEx
*   ����˵��: ��ʾBMPͼƬ
*   ��    �Σ�sFilename Ҫ��ʾͼƬ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t EmWinXslApp_ShowBMP(const char * sFilename,uint16_t xpos,uint16_t ypos)
{
    //OS_ERR          err;
    // ���ļ�
    ModuleSdFat_Res = f_open(&ModuleSdFat_fsrc, sFilename, FA_OPEN_EXISTING | FA_READ /*| FA_OPEN_ALWAYS*/);
    if (ModuleSdFat_Res != FR_OK)
    {
        return ERR;
    }
    // ���뻺��(�����ڴ���: ÿ�ι̶���ʾһ��,ÿ������4�ֽ� ��320*4=1280B)
    pGuiBuffer = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    //  XSize = GUI_BMP_GetXSizeEx(_GetData, &file);
    //  YSize = GUI_BMP_GetYSizeEx(_GetData, &file);
    //OSSchedLock(&err);
    GUI_BMP_DrawEx(EmWinXslApp_GetTfData, &ModuleSdFat_fsrc, xpos, ypos);
    //OSSchedUnlock(&err);
    // �ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pGuiBuffer);
    // �ر��ļ�
    f_close(&ModuleSdFat_fsrc);
    //
    return OK;
}

/*
*********************************************************************************************************
*   �� �� ��: _ShowJPGEx
*   ����˵��: ��ʾBMPͼƬ
*   ��    �Σ�sFilename Ҫ��ʾͼƬ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t EmWinXslApp_ShowJPG(const char * sFilename,uint16_t xpos,uint16_t ypos)
{
    //OS_ERR          err;
    uint16_t i16;
    static GUI_JPEG_INFO JpegInfo;
    // ���ļ�
    ModuleSdFat_Res = f_open(&ModuleSdFat_fsrc, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
    if (ModuleSdFat_Res != FR_OK)
    {
        return ERR;
    }
    // ���뻺��
    pGuiBuffer = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    //  XSize = GUI_BMP_GetXSizeEx(_GetData, &file);
    //  YSize = GUI_BMP_GetYSizeEx(_GetData, &file);
    //
    GUI_JPEG_GetInfoEx(EmWinXslApp_GetTfData,&ModuleSdFat_fsrc,&JpegInfo);
    //*��Ҫ��ʱ����ϵ�����,��ֹˢͼƬ��ʱ������
    for(i16=100; i16<200; i16+=10)
    {
        //OSSchedLock(&err);
        GUI_JPEG_DrawScaledEx(EmWinXslApp_GetTfData,\
                              &ModuleSdFat_fsrc,\
                              (LCD_GetXSize()-JpegInfo.XSize*i16/100)/2,\
                              (LCD_GetYSize()-JpegInfo.YSize*i16/100)/2,\
                              i16,\
                              100);
        //OSSchedUnlock(&err);
        GUI_Delay(1000);
    }
    // �ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pGuiBuffer);
    // �ر��ļ�
    f_close(&ModuleSdFat_fsrc);
    //
    return OK;
}
/*
*********************************************************************************************************
*   ����˵��: ��ʾBMPͼƬ
*   ��    �Σ�sFilename Ҫ��ʾͼƬ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void EmWinXslApp_ShowGIF(const char * sFilename,uint16_t xpos,uint16_t ypos)
{
    //OS_ERR  err;
    static GUI_GIF_INFO         InfoGif1;
    static GUI_GIF_IMAGE_INFO   InfoGif2;
    uint8_t i=0;
    // ���ļ�
    ModuleSdFat_Res = f_open(&ModuleSdFat_fsrc, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
    if (ModuleSdFat_Res != FR_OK)
    {
        return;
    }
    // ���뻺��
    pGuiBuffer = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    //  XSize = GUI_BMP_GetXSizeEx(_GetData, &file);
    //  YSize = GUI_BMP_GetYSizeEx(_GetData, &file);
    //
    GUI_GIF_GetInfoEx(EmWinXslApp_GetTfData,&ModuleSdFat_fsrc,&InfoGif1);
    // ��Ҫ��ʱ����ϵ�����,��ֹˢͼƬ��ʱ������
    //OSSchedLock(&err);
    while(1)
    {
        if(i<InfoGif1.NumImages)
        {

            GUI_GIF_GetImageInfoEx(EmWinXslApp_GetTfData,&ModuleSdFat_fsrc,&InfoGif2,i);
            GUI_GIF_DrawSubEx(EmWinXslApp_GetTfData,\
                              &ModuleSdFat_fsrc,\
                              (LCD_GetXSize()-InfoGif1.xSize)/2,\
                              (LCD_GetYSize()-InfoGif1.ySize)/2,\
                              i++);
            GUI_X_Delay(InfoGif2.Delay*10);
        }
        else
        {
            break;
        }
    }
    //OSSchedUnlock(&err);
    // �ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pGuiBuffer);
    // �ر��ļ�
    f_close(&ModuleSdFat_fsrc);
}
/*
*********************************************************************************************************
*   �� �� ��: _ShowPNGEx
*   ����˵��: ��ʾPNGͼƬ
*   ��    �Σ�sFilename Ҫ��ʾͼƬ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
/*
void EmWinXslApp_ShowPNG(const char * sFilename,uint16_t xpos,uint16_t ypos)
{
    //OS_ERR  err;
    // ���ļ�
    ModuleSdFat_Res = f_open(&ModuleSdFat_fsrc, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
    if (ModuleSdFat_Res != FR_OK)
    {
        return;
    }
    // ���뻺��
    //pGuiBuffer = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    //  XSize = GUI_BMP_GetXSizeEx(_GetData, &file);
    //  YSize = GUI_BMP_GetYSizeEx(_GetData, &file);
    //
    //��Ҫ��ʱ����ϵ�����,��ֹˢͼƬ��ʱ������
    //OSSchedLock(&err);
    GUI_PNG_DrawEx(_GetData,&ModuleSdFat_fsrc,xpos,ypos);
    //OSSchedUnlock(&err);
    // �ͷŻ���
    //MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pGuiBuffer);
    // �ر��ļ�
    f_close(&ModuleSdFat_fsrc);
}
*/
/*
*********************************************************************************************************
*   �� �� ��: _cbGetData
*   ����˵��: ʹ��XBF����
*   ��    ��: Off       Ҫ��ȡ���ݵ���ʼλ��
*             NumBytes  Ҫ��ȡ���ֽ���
*             pVoid     Ӧ�ö���ָ�룬�˴����ļ����ָ��
*             pBuffer   ���ڴ洢�������ݵĵ�ַ
*   �� �� ֵ: 0 �ɹ�  1 ʧ��
*********************************************************************************************************
*/
GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;
FIL          Fontfile;
static int EmWinXslApp_GetTfXbfData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer)
{
    FIL *FontFile;
    UINT NumBytesRead;
    // �ļ����
    FontFile = (FIL *)pVoid;
    // ָ�����ݵ�ַ
    ModuleSdFat_Res =f_lseek(FontFile, Off);
    if (ModuleSdFat_Res != FR_OK)
    {
        return 1;
    }
    // ��ȡ���ݵ�buffer
    ModuleSdFat_Res = f_read(FontFile, pBuffer, NumBytes, &NumBytesRead);
    if (ModuleSdFat_Res != FR_OK)
    {
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*   �� �� ��: _ShowXBF
*   ����˵��: ʹ��XBF����
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void EmWinXslApp_ShowXBF(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        ModuleSdFat_Res = f_open(&Fontfile, "yh36.xbf", FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
        if (ModuleSdFat_Res != FR_OK)
        {
            return;
        }

        /* ����XBF���� */
        GUI_XBF_CreateFont(&XBF_Font,
                           &XBF_Data,
                           GUI_XBF_TYPE_PROP_AA4_EXT,
                           EmWinXslApp_GetTfXbfData,
                           &Fontfile);
    }
    else if(OnOff==OFF)
    {
        f_close(&Fontfile);
    }
}
/*
*********************************************************************************************************
*   ����˵��: ����
*********************************************************************************************************
*/
void EmWinXslApp_Test(void)
{
    // ʹ���ڴ��豸��ֹ����
    WM_SetCreateFlags(WM_CF_MEMDEV);
    //--------------------ʵ��һ:��������ʵ��
    /*
    ���ú���
      GUI_DispString("Xsl \nTest text!");
      GUI_DispStringAt();
      GUI_DispStringHCenterAt("Xsl \nTest text!",120,120);
      GUI_DispStringInRect();
    */
#if EMWIN_XSL_APP_TEST_BASIC_TEXT
    {
        {
            int i;
            GUI_SetBkColor(GUI_BLUE);
            GUI_Clear();
            GUI_SetPenSize(10);
            GUI_SetColor(GUI_RED);
            GUI_DrawLine(80,20,240,100);
            GUI_DrawLine(80,100,240,20);
            //
            i=0;
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(&GUI_Font16B_1);
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_DispStringHCenterAt("TEST->TEXT",160,i);
            //
            GUI_SetBkColor(GUI_BLACK);
            GUI_SetColor(GUI_RED);
            GUI_SetFont(&GUI_Font8x16);
            //�����ı�
            i=20;
            GUI_SetTextMode(GUI_TM_NORMAL);
            GUI_DispStringHCenterAt("GUI_TM_NORMAL",160,i);
            //��ת�ı�
            i+=16;
            GUI_SetTextMode(GUI_TM_REV);
            GUI_DispStringHCenterAt("GUI_TM_REV",160,i);
            //͸���ı�
            i+=16;
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_DispStringHCenterAt("GUI_TM_TRANS",160,i);
            //����ı�
            i+=16;
            GUI_SetTextMode(GUI_TM_XOR);
            GUI_DispStringHCenterAt("GUI_TM_XOR",160,i);
            //�ۺ��ı�
            i+=16;
            GUI_SetTextMode(GUI_TM_TRANS | GUI_TM_REV);
            GUI_DispStringHCenterAt("GUI_TM_TRANS | GUI_TM_REV",160,i);
            //�����ı�
            i+=16;
            GUI_SetTextMode(GUI_TM_NORMAL);
            GUI_SetTextStyle(GUI_TS_NORMAL);
            GUI_DispStringHCenterAt("GUI_TS_NORMAL",160,i);
            //�»����ı�
            i+=16;
            GUI_SetTextStyle(GUI_TS_UNDERLINE);
            GUI_DispStringHCenterAt("GUI_TS_UNDERLINE",160,i);
            //ɾ�����ı�
            i+=16;
            GUI_SetTextStyle(GUI_TS_STRIKETHRU);
            GUI_DispStringHCenterAt("GUI_TS_STRIKETHRU",160,i);
            //�ϻ����ı�
            i+=16;
            GUI_SetTextStyle(GUI_TS_OVERLINE);
            GUI_DispStringHCenterAt("GUI_TS_OVERLINE",160,i);
            //
            GUI_Delay(5000);
        }
        {
            //����
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(0,20,320,240);
            //
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_SetTextStyle(GUI_TS_NORMAL);
            GUI_SetColor(GUI_YELLOW);
            //
            GUI_SetFont(&GUI_Font8_ASCII);
            GUI_DispStringAt("Hello World",0,20+0);
            GUI_SetFont(&GUI_Font10S_ASCII);
            GUI_DispStringAt("Hello World",0,20+30);
            GUI_SetFont(&GUI_Font13_1);
            GUI_DispStringAt("Hello World",0,20+60);
            GUI_SetFont(&GUI_Font16_ASCII);
            GUI_DispStringAt("Hello World",0,20+90);
            GUI_SetFont(&GUI_FontComic18B_ASCII);
            GUI_DispStringAt("Hello World",0,20+120);
            GUI_SetFont(&GUI_FontComic24B_ASCII);
            GUI_DispStringAt("Hello World",0,20+150);
            GUI_SetFont(&GUI_Font32B_ASCII);
            GUI_DispStringAt("Hello World",0,20+180);
            //
            GUI_Delay(5000);
        }
        /*
        ʵ��Ŀ��: ��ʾ�Զ����ֿ�(c�ļ�)������
        ��ʾ����:
            1,��������:   FontCvtDemo(StEmWin�ļ����еĲ�������)
            2,���������������C�ֿ��ļ�
               �½�txt�ı���д�뺺�֣�����Ϊunicode�����ļ�
               FontCvtDemoѡ�������½��ֿ⣬���������ַ�(Edit�˵�)
               FontCvtDemo����txt�ļ�(Edit-Read pattern file...)
               FontCvtDemo����Ϊc�����ļ�
            3, �������ĵ��ļ�����ΪUTF8����,ͳһ������LanguageUtf8.c
        */
        {
            extern GUI_CONST_STORAGE GUI_FONT GUI_FontXslZk;
            //����
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(0,20,320,240);
            //
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_SetTextStyle(GUI_TS_NORMAL);
            GUI_SetColor(GUI_YELLOW);
            //
            GUI_UC_SetEncodeUTF8();
            GUI_SetFont(&GUI_FontXslZk);
            GUI_DispStringAt((char*)pSTR_UTF8_XSLZK[Main_Language],0,20+0);
            //
            GUI_Delay(5000);
        }
        /*
        ʵ��Ŀ��: ��ʾ�ⲿ�ֿ�(xbf�ļ�)������
        ��ʾ����:
            1,��������:   FontCvtDemo(StEmWin�ļ����еĲ�������)
            2,���������������xbf�ֿ��ļ�
               FontCvtDemoѡ�������½��ֿ⣬���������ַ�(Edit�˵�)
               FontCvtDemo����Ϊxbf�����ļ�
            3, �������ĵ��ļ�����ΪUTF8����,ͳһ������LanguageUtf8.c
        */
        {
            //����
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(0,20,320,240);
            //
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_SetTextStyle(GUI_TS_NORMAL);
            GUI_SetColor(GUI_GREEN);
            //
            //���ش���
            ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
            if( ModuleSdFat_Res != FR_OK )
            {
                return;
            }
            EmWinXslApp_ShowXBF(ON);
            GUI_UC_SetEncodeUTF8();
            GUI_SetFont(&XBF_Font);
            GUI_DispStringAt((char*)pSTR_UTF8_XSLZK[Main_Language],0,20+0);
            EmWinXslApp_ShowXBF(OFF);
            //ж�ش���
            ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
            //
            GUI_Delay(5000);
        }
    }
#endif
    //--------------------ʵ���:������ʾ����ʵ��
#if EMWIN_XSL_APP_TEST_WRAP_TEXT
    {
        int i;
        char pstr[]="this example demonstrates text wrapping";
        GUI_RECT rect = {10,20,59,69};
        GUI_WRAPMODE aWm[]  =   {GUI_WRAPMODE_NONE,GUI_WRAPMODE_CHAR,GUI_WRAPMODE_WORD};
        //
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->Text Wrapping",160,0);
        //
        GUI_SetFont(&GUI_Font6x8);
        //
        for(i=0; i<3; i++)
        {
            GUI_SetColor(GUI_BLUE);
            GUI_FillRectEx(&rect);
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringInRectWrap(pstr,&rect,GUI_TA_LEFT,aWm[i]);
            rect.x0+=60;
            rect.x1+=60;
        }
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------ʵ����:��ֵ��ʾ
#if EMWIN_XSL_APP_TEST_DISPDEC
    {
        int i;
        //
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->DispDec",160,0);
        //
        GUI_SetFont(&GUI_Font24B_ASCII);
        //��ͨ��ʾ
        i=20;
        GUI_GotoXY(0,i);
        GUI_DispDec(-123,4);
        GUI_GotoXY(100,i);
        GUI_DispDec(1234,7);
        //ָ��λ����ʾ
        i+=24;
        GUI_DispDecAt(1234,0,i,4);
        //�����ַ���ʾ(����ָ������)
        i+=24;
        GUI_GotoXY(0,i);
        GUI_DispDecMin(-12345);
        //��ʾ����ģʽ��С��
        GUI_GotoXY(100,i);
        GUI_DispDecShift(-12345,7,2);
        //��ʾ����(ǿ����ʾ������)
        i+=24;
        GUI_GotoXY(0,i);
        GUI_DispSDec(12345,6);
        //��ʾ����ģʽ��С��(ǿ����ʾ������)
        GUI_GotoXY(100,i);
        GUI_DispSDecShift(12345,7,2);
        //��ʾʮ�����ƺͶ�����
        i+=24;
        GUI_DispBinAt(0x55,0,i,8);
        GUI_DispHexAt(0x5A,100,i,2);
        //��ʾ������ֵ
        i+=24;
        GUI_GotoXY(0,i);
        GUI_DispSFloatMin(-12.345,3);
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------ʵ������2D������ͼ
    /*
    ���ú�����
        ���ʣ�
        GUI_GetPenSize()
        GUI_SetPenSize()
        ʸ����ͼ��
        GUI_DrawPoint()
        GUI_DrawLine()
        GUI_DrawLineRel()
        GUI_DrawLineTo()
        GUI_DrawPolyLine()
        GUI_DrawPolygon()
        GUI_DrawEllipse()
        GUI_DrawArc()
        ������ͼ��
        GUI_ClearRect()
        GUI_CopyRect()
        GUI_DrawGradientH()
        GUI_DrawGradientV()
        GUI_DrawGradientRoundedH()
        GUI_DrawGradientRoundedV()
        GUI_DrawPixel()
        GUI_DrawPoint()
        GUI_DrawRect()
        GUI_DrawRectEx()
        GUI_DrawRoundedFrame()
        GUI_DrawRoundedRect()
        GUI_FillRect()
        GUI_FillRectEx()
        GUI_FillRoundedRect()
        GUI_InvertRect()
        ��������
        GUI_DrawPolygon()       //���ƶ���ε�����
        GUI_EnlargePolygon()    //��չ�����
        GUI_FillPolygon()       //�������Ķ����
        GUI_MagnifyPolygon()    //�Ŵ�����
        GUI_RotatePolygon()     //��ָ���Ƕ���ת�����
    */
#if EMWIN_XSL_APP_TEST_BASIC_2D
    {
        int i,j,k;
        //
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->2D Basic",160,0);
        //����
        GUI_SetColor(GUI_RED);
        i=20;
        GUI_SetPenSize(1);
        GUI_DrawLine(0,i,200,i);
        i+=10;
        GUI_SetPenSize(4);
        GUI_DrawLine(0,i,200,i);
        i+=10;
        GUI_SetPenSize(8);
        GUI_DrawLine(0,i,200,i);
        //������
        i+=10;
        GUI_SetColor(GUI_RED);
        GUI_DrawGradientRoundedH(0,i+0,50,i+50,25,0x0000FF,0x00FFFF);
        GUI_DrawRect(60,i+0,110,i+50);
        GUI_DrawRoundedFrame(120,i+0,170,i+50,10,10);
        GUI_FillRoundedRect(180,i+0,230,i+50,10);
        //��Alphaͼ��(͸���ȿ��趨)
        i+=60;
        GUI_EnableAlpha(1);
        GUI_SetAlpha(0);
        GUI_SetColor(GUI_BLUE);
        GUI_FillCircle(100,i+50,49);
        GUI_SetColor(GUI_YELLOW);
        for(j=0; j<100; j++)
        {
            k=(j*255)/100;
            GUI_SetAlpha(k);
            GUI_DrawHLine(i+j,100-j,100+j);
        }
        GUI_SetAlpha(0x80);
        GUI_SetColor(GUI_MAGENTA);
        GUI_SetFont(&GUI_Font24B_ASCII);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_DispStringHCenterAt("Alphablending",100,i+3);
        GUI_SetAlpha(0);
        GUI_EnableAlpha(0);
        //��Բ
        GUI_SetColor(GUI_GREEN);
        for(i=10; i<=50; i+=3)
        {
            GUI_DrawCircle((320/4)*3,(240/4)*3,i);
        }
        //
        GUI_Delay(5000);
        //�ö���κ�����-----��չ������
        {
            int i;
            const GUI_POINT POINT_BUF[]= {{40,20},{0,20},{20,0}};
            GUI_POINT pointbuf[GUI_COUNTOF(POINT_BUF)];
            //����
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(0,20,320,240);
            GUI_SetDrawMode(GUI_DM_XOR);
            GUI_FillPolygon(POINT_BUF,GUI_COUNTOF(POINT_BUF),140,110);
            for(i=0; i<10; i++)
            {
                GUI_EnlargePolygon(pointbuf,POINT_BUF,GUI_COUNTOF(POINT_BUF),i*5);
                GUI_FillPolygon(pointbuf,GUI_COUNTOF(POINT_BUF),140,110);
            }
            GUI_SetDrawMode(GUI_DM_NORMAL);
            GUI_Delay(5000);
        }
        //����ͼ
        {
            int i,a0,a1;
            const unsigned aValues[]= {100,135,190,240,340,360};
            const GUI_COLOR aColors[]= {GUI_BLUE,GUI_GREEN,GUI_RED,GUI_CYAN,GUI_MAGENTA,GUI_YELLOW};
            //����
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(0,20,320,240);
            //
            for(i=0; i<GUI_COUNTOF(aValues); i++)
            {
                a0=(i==0)?0:aValues[i-1];
                a1=aValues[i];
                GUI_SetColor(aColors[i]);
                GUI_DrawPie(320/2,240/2,100,a0,a1,0);
            }
            //
            GUI_Delay(5000);
        }
    }
#endif
    //--------------------ʵ�飺bmp��ʾ(���ص��洢��)
    /*
    ���ú���:
        GUI_BMP_Draw()             �����Ѽ��ص��洢����BMP�ļ�
        GUI_BMP_DrawEx()           ����������ص��洢����BMP�ļ�
        GUI_BMP_DrawScaled()       �����Ѽ��ص��洢���Ĵ�������BMP�ļ�
        GUI_BMP_DrawScaledEx()  ����������ص��洢���Ĵ�������BMP�ļ�
        GUI_BMP_GetXSize()      ���ؼ��ص��洢����BMP�ļ���X��С
        GUI_BMP_GetXSizeEx()       ����������ص��洢����BMP�ļ���X��С
        GUI_BMP_GetYSize()      ���ؼ��ص��洢����BMP�ļ���Y��С
        GUI_BMP_GetYSizeEx()       ����������ص��洢����BMP�ļ���Y��С
        GUI_BMP_Serialize()     ����BMP�ļ�
        GUI_BMP_SerializeEx()   ���ڸ����ľ��δ���BMP�ļ�
    */
#if EMWIN_XSL_APP_TEST_BMP
    {
        extern GUI_CONST_STORAGE GUI_BITMAP bm1;
        extern GUI_CONST_STORAGE GUI_BITMAP bmres2;
        //
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->BMP",160,0);
        //
        GUI_DrawBitmap(&bm1,0,0);
        GUI_DrawBitmap(&bm1,320-72,0);
        GUI_DrawBitmap(&bmres2,0,240-181);
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------ʵ�飺����TF�����bmpͼƬ
#if EMWIN_XSL_APP_TEST_BMP_TF
    /*
    �α꺯��:
      GUI_CURSOR_Select()        �α�ѡ��
      GUI_CURSOR_Show()          �α���ʾ
      GUI_CURSOR_SetPosition()   �����α�λ��
      GUI_CURSOR_Hide()          �α�����
    */
    {
        uint16_t i16;
        int t32;
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->BMP(TF card)",160,0);
        //
        //���ش���
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        EmWinXslApp_ShowBMP("res3.bmp",0,20);
        //ж�ش���
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
        //��ʾ�α�
        GUI_CURSOR_Show();
        GUI_CURSOR_SetPosition(100,100);
        //
        GUI_CURSOR_SelectAnim(&GUI_CursorAnimHourglassM);
        GUI_Delay(5000);
        GUI_CURSOR_Select(&GUI_CursorArrowM);
        //�������α�λ�õķ�ʽ�����α�
        for(i16=1; i16<=320; i16++)
        {
            t32 = GUI_GetTime();
            GUI_CURSOR_SetPosition(i16,100);
            while((GUI_GetTime()-t32)<10);
        }
        //��ʸ����ʽ�����α�
#if 0				
        {
            GUI_PID_STATE guiPidState;
            guiPidState.x=1;
            guiPidState.y=200;
            GUI_PID_StoreState(&guiPidState);
            for(i16=1; i16<=300; i16++)
            {
                t32 = GUI_GetTime();
                GUI_PID_GetState(&guiPidState);
                guiPidState.x+=1;
                GUI_PID_StoreState(&guiPidState);
                GUI_Exec();
                while((GUI_GetTime()-t32)<10);
            }
        }
#endif				
        //GUI_Delay(5000);
        GUI_CURSOR_Hide();
    }
#endif
    //--------------------ʵ�飺����TF�����JPGͼƬ
#if EMWIN_XSL_APP_TEST_JPG_TF
    {
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->JPEG(TF card)",160,0);
        //
        //���ش���
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        EmWinXslApp_ShowJPG("res4.jpg",0,20);
        //ж�ش���
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------ʵ�飺����TF�����GIFͼƬ(��)
#if EMWIN_XSL_APP_TEST_GIF_TF
    {
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->GIF(TF card)",160,0);
        //
        //���ش���
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        EmWinXslApp_ShowGIF("res5.gif",0,20);
        //ж�ش���
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------ʵ�飺����TF�����PNGͼƬ(δʵ��ɹ�,��PNG������Ҫ�������������,���Ժ�����)
#if EMWIN_XSL_APP_TEST_PNG_TF
    {
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        //
        GUI_SetColor(GUI_YELLOW);
        GUI_SetFont(&GUI_Font16B_1); //
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_DispStringHCenterAt("TEST->PNG(TF card)",160,0);
        //
        //���ش���
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        //EmWinXslApp_ShowPNG("res1.png",0,20);
        EmWinXslApp_ShowPNG("res6.png",0,100);
        EmWinXslApp_ShowPNG("res7.png",100,100);
        //ж�ش���
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
        //
        GUI_Delay(5000);
    }
#endif
    //--------------------����ʵ��
    /*
    ���ú���:
      GUI_StoreKeyMsg()       ����Ϣ�洢��ָ����
      GUI_SendKeyMsg()        ����Ϣ������ָ����
         GUI_ClearKeyBuffer() �����������
         GUI_GetKey()         ���ؼ��������е�����
         GUI_StoreKey()       �Ѽ��洢�ڻ�������
         GUI_WaitKey()        �ȴ���������
    */
    //--------------------
    BspKey_NewSign=0;
    while(1)
    {
        if(BspKey_NewSign==1)
        {
            BspKey_NewSign=0;
            if(BspKey_Value==HCI_KEY_ESC)
            {
               break;
            }
        }
        GUI_Delay(300);
    }
}
//-------------------------------------------------------------------------------------------------------
