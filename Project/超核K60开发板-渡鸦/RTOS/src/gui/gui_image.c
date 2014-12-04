#include "DIALOG.h"
#include <dfs_posix.h>
#include <stdbool.h>
#include "gui_appdef.h"
#define ID_FRAMEWIN_0       (GUI_ID_USER + 0x00)
#define ID_IMAGE_0          (GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "Image", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, FRAMEWIN_CF_MOVEABLE, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 50, 50, 0, IMAGE_CF_AUTOSIZE|IMAGE_CF_TILE | IMAGE_CF_ALPHA, 0 },
};

static unsigned char *_acBuffer = RT_NULL;
static const char *gpath;
static U32 gFileSize;
static int fpic;
static bool gIsReadOver;

int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off)
{
    rt_uint32_t NumBytesRead;
    
    if (NumBytes >512) NumBytes = 512;
    lseek(fpic, Off, SEEK_SET);
    NumBytesRead = read(fpic, _acBuffer, NumBytes);
    if(NumBytesRead == -1)
    {
        rt_kprintf("read failed\r\n");
    }
    //rt_kprintf("NumBytesRead:%d Off:%d\r\n", NumBytesRead, Off);
    if(gFileSize < 512)
    {
        //rt_kprintf("close1\r\n");
        gIsReadOver = true;
        close(*(int*)(p));
    }
    else if(gFileSize - (Off + NumBytes) < 100 )
    {
        ///rt_kprintf("close2\r\n");
        gIsReadOver = true;
        close(*(int*)(p));
    }
    *ppData = _acBuffer;
    return NumBytesRead;
}


static int _DispImage(WM_HWIN Handle, const char *path)
{

    U32 fileSize, xSize, ySize;
    struct stat f_stat;
    fpic = open(path, O_RDONLY , 0);
    if(fpic <0)
    {
        rt_kprintf("open file failed\r\n");
        return 1;
    }
    stat(path, &f_stat);
    gFileSize = f_stat.st_size;
    fileSize = f_stat.st_size;
    _acBuffer = rt_malloc(512);
    read(fpic, _acBuffer, 512);
    rt_kprintf("Size:%d\r\n", gFileSize);
    gIsReadOver = false;
    switch(GUI_AppGetFileType(path))
    {
        case kBMP:
        xSize = GUI_BMP_GetXSize(_acBuffer);
        ySize = GUI_BMP_GetYSize(_acBuffer);
        IMAGE_SetBMPEx(Handle, _GetData, (void*)&fpic);
        while(gIsReadOver == false)
        {
            GUI_Exec();
        }
            break;
        case kJPG:
        case kJPEG:
    /* try handle JPG file */
    static GUI_JPEG_INFO Info;
    if(!GUI_JPEG_GetInfo(_acBuffer, fileSize, &Info))
    {
        xSize = Info.XSize;
        ySize = Info.YSize;
        rt_kprintf("xSize:%d ySize:%d\r\n", xSize, ySize);
        IMAGE_SetJPEGEx(Handle, _GetData, (void*)&fpic);
    }
            break;
       
    }
    rt_free(_acBuffer);
    return 0;
    /* resize window*/
  //  if(xSize > LCD_GetXSize()) xSize = LCD_GetXSize();
  //  if(ySize > LCD_GetYSize()) ySize = LCD_GetYSize();
  //  WM_SetSize(Handle, xSize, ySize);
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN      hItem;
    int     NCode;
    int     Id;
    static int _x;
    //rt_kprintf("pMsg->MsgID:%d\r\n", pMsg->MsgId);
    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        hItem = pMsg->hWin;
        FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
        FRAMEWIN_AddMaxButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
        WM_MakeModal(hItem);
  
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);

        _DispImage(hItem, gpath);
    
        break;
    default:
        WM_DefaultProc(pMsg);
        break; 
    }
}


void GUI_AppDispImage(const char * path)
{
    int r;
    gpath = path;
    r = GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return;
}

