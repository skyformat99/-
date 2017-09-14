#include <stdio.h>     
#include <windows.h>  
//#include "dirent.h"
#include"NewDir.h"

static HANDLE hFind;

DIR *opendir(const char *name)
{
	DIR *dir;
	WIN32_FIND_DATA FindData;
	char namebuf[512];
	
	sprintf_s(namebuf, 512,"%s\\*.*", name);
	printf("namebuf:%s\n", namebuf);
//	hFind = FindFirstFile("..\\*.*", &FindData);
	hFind = FindFirstFile(namebuf, &FindData);
	//FindFiles(namebuf);
	/*if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed (%d)\n", GetLastError());
		return 0;
	}
	*/
	dir = (DIR *)malloc(sizeof(DIR));
	if (!dir)
	{
		printf("DIR memory allocate fail\n");
		return 0;
	}

	memset(dir, 0, sizeof(DIR));
	dir->dd_fd = 0;   // simulate return  

	return dir;
};
void FindFiles(char* strPath)

{
	WIN32_FIND_DATA findData;

	char strTemp[512];
	//strTemp.Format(_T("%s\\*.*"), strPath);//查找指定目录下的直接的所有文件和目录
	sprintf_s(strTemp, 512, "%s\\*.*", strPath);
	HANDLE hFile = FindFirstFile(strTemp, &findData);
	while (hFile != INVALID_HANDLE_VALUE)
	{
		printf("opensuccess\n");
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//如果是目录
		{
			if (findData.cFileName[0] != '.')//排除.和..文件夹
			{

				//strTemp = strPath + CString("\\") + findData.cFileName;//获得完整路径
				strcat_s(strPath, sizeof("\\"), "\\");
				strcat_s(strPath, sizeof(findData.cFileName), findData.cFileName);

				FindFiles(strTemp);//递归查找当前目录的文件
			}
		}
		else
		{
			//strTemp = strPath + CString("\\") + findData.cFileName;
			strcat_s(strPath, sizeof("\\"), "\\");
			strcat_s(strPath, sizeof(findData.cFileName), findData.cFileName);

		}

		if (!FindNextFile(hFile, &findData))
		{
			break;
		}
	}

}
struct dirent *readdir(DIR *d)
{
	int i;
	static struct dirent dirent;
	BOOL bf;
	WIN32_FIND_DATA FileData;
	if (!d)
	{
		return 0;
	}

	bf = FindNextFile(hFind, &FileData);
	//fail or end  
	if (!bf)
	{
		return 0;
	}

	for (i = 0; i < 256; i++)
	{
		dirent.d_name[i] = FileData.cFileName[i];
		if (FileData.cFileName[i] == '\0') break;
	}
	dirent.d_reclen = i;
	dirent.d_reclen = FileData.nFileSizeLow;

	//check there is file or directory  
	if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		dirent.d_type = 2;
	}
	else
	{
		dirent.d_type = 1;
	}


	return (&dirent);
}
/*            备用函数
struct dirent *readdir(DIR *d)
{
int i;

BOOL bf;
WIN32_FIND_DATA FileData;
if(!d)
{
return 0;
}

bf=FindNextFile(hFind,&FileData);
//fail or end
if(!bf)
{
return 0;
}

struct dirent *dirent = (struct dirent *)malloc(sizeof(struct dirent)+sizeof(FileData.cFileName));

for(i = 0; i < 256; i++)
{
dirent->d_name[i] = FileData.cFileName[i];
if(FileData.cFileName[i] == '\0') break;
}
dirent->d_reclen = i;
dirent->d_reclen = FileData.nFileSizeLow;

//check there is file or directory
if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
{
dirent->d_type = 2;
}
else
{
dirent->d_type = 1;
}

return dirent;
}
*/
int closedir(DIR *d)
{
	if (!d) return -1;
	hFind = 0;
	free(d);
	return 0;
}