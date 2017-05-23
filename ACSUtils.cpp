#include "ACSUtils.h"
#include "ACSConstants.h"

#include <QFile>
#include <QSettings>
#include <windows.h>

ACSUtils::ACSUtils()
{
}

ACSUtils::~ACSUtils()
{
}

void GetLicense(char *host_id, char *license);

bool ACSUtils::CheckSoftwareLicense()
{
	char license[30];
	const char *filename;

	if (QFile::exists(LICENSE_FILE_32))
	{
		filename = LICENSE_FILE_32;
	}
	else if (QFile::exists(LICENSE_FILE_64))
	{
		filename = LICENSE_FILE_64;
	}
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
		return false;
	fread(license, 29, 1, f);
	fclose(f);

	DWORD serial_num = 0;                                 //Ӳ�����к�
	QString lpRootPathName = "D:\\";
	LPTSTR lpVolumeNameBuffer = new TCHAR[12];            //���̾��
	DWORD nVolumeNameSize = 12;                           //�����ַ�������
	DWORD MaximumComponentLength;                       //�����ļ�����
	LPTSTR lpFileSystemNameBuffer = new TCHAR[10];        //�洢�����̷��ķ������͵ĳ�ָ�����
	DWORD nFileSystemNameSize = 10;                       //�������͵ĳ�ָ�������ָ����ַ�������
	DWORD FileSystemFlags;                              //�ļ�ϵͳ��һ�˱�־

	GetVolumeInformation((LPTSTR)lpRootPathName.utf16(),
		lpVolumeNameBuffer, nVolumeNameSize,
		&serial_num, &MaximumComponentLength,
		&FileSystemFlags,
		lpFileSystemNameBuffer, nFileSystemNameSize);

	char host_id[20];
	sprintf(host_id, "%010u", serial_num);
	char Lic[20];
	GetLicense(host_id, Lic);
	for (int i = 0; i < 14; i++)
		if (license[i] != Lic[i])
		{
			delete lpVolumeNameBuffer;
			delete lpFileSystemNameBuffer;
			return false;
		}

	delete lpVolumeNameBuffer;
	delete lpFileSystemNameBuffer;
	return true;
}

bool ACSUtils::CheckSoftwareRegistry()
{
	QSettings *reg = new QSettings("HKEY_LOCAL_MACHINE\\Software\\XPlan", QSettings::NativeFormat);
	QString str = reg->value("ProductKey", 0).toString();
	delete reg;

	DWORD serial_num = 0;                                 //Ӳ�����к�
	QString lpRootPathName = "D:\\";
	LPTSTR lpVolumeNameBuffer = new TCHAR[12];            //���̾��
	DWORD nVolumeNameSize = 12;                           //�����ַ�������
	DWORD MaximumComponentLength;                       //�����ļ�����
	LPTSTR lpFileSystemNameBuffer = new TCHAR[10];        //�洢�����̷��ķ������͵ĳ�ָ�����
	DWORD nFileSystemNameSize = 10;                       //�������͵ĳ�ָ�������ָ����ַ�������
	DWORD FileSystemFlags;                              //�ļ�ϵͳ��һ�˱�־

	GetVolumeInformation((LPTSTR)lpRootPathName.utf16(),
		lpVolumeNameBuffer, nVolumeNameSize,
		&serial_num, &MaximumComponentLength,
		&FileSystemFlags,
		lpFileSystemNameBuffer, nFileSystemNameSize);
	char string[50];
	sprintf(string, "%010u", serial_num);
	char lic[50];
	memset(lic, 0, 50);
	GetLicense(string, lic);
	if (str == lic)
	{
		delete lpVolumeNameBuffer;
		delete lpFileSystemNameBuffer;
		return true;
	}

	delete lpVolumeNameBuffer;
	delete lpFileSystemNameBuffer;
	return false;
}

void GetLicense(char *host_id, char *license)
{
	//code matrix
	//int A[]={
	//	1,1,5,2,1,1,3,1,0,1,
	//	2,0,1,0,1,2,1,0,5,0,
	//	1,0,2,1,1,1,1,1,2,5,
	//	0,1,1,2,0,1,2,5,1,0,
	//	1,1,0,0,5,0,0,2,0,1,
	//	0,2,1,1,1,2,5,1,1,3,
	//	2,5,1,0,0,2,0,1,0,0,
	//	1,1,1,5,1,0,1,0,3,1,
	//	0,0,0,1,1,5,1,1,0,1,
	//	5,1,2,2,2,1,1,1,1,0
	//};
	//new version
	int A[] = {
		1, 1, 5, 2, 1, 1, 3, 1, 0, 1,
		1, 0, 2, 1, 1, 1, 1, 1, 2, 5,
		0, 1, 1, 2, 0, 1, 2, 5, 1, 0,
		0, 2, 1, 1, 1, 2, 5, 1, 1, 3,
		2, 5, 1, 0, 0, 2, 0, 1, 0, 0,
		1, 1, 0, 0, 5, 0, 0, 2, 0, 1,
		1, 1, 1, 5, 1, 0, 1, 0, 3, 1,
		0, 0, 0, 1, 1, 5, 1, 1, 0, 1,
		2, 0, 1, 0, 1, 2, 1, 0, 5, 0,
		5, 1, 2, 2, 2, 1, 1, 1, 1, 0
	};
	int code[12];
	//����10λ����

	int i;
	for (i = 0; i < 10; i++){
		code[i] = 0;
		for (int j = 0; j < 10; j++){
			int xj = host_id[j] - '0';
			code[i] += A[i * 10 + j] * xj;
		}
	}
	//11��12λ����Ϊ��ǰ���ۼ����
	code[10] = code[11] = 0;

	for (i = 0; i < 10; i++)
		if (i % 2)
			code[10] += code[i];
		else
			code[11] += code[i];
	//��62=10+26*2ȡ��
	for (i = 0; i < 12; i++)
		code[i] = code[i] % 62;
	int ip = 0;
	for (i = 0; i < 12; i++, ip++){
		//4���ַ�һ����Ԫ
		if (i > 0 && i % 4 == 0){
			license[ip] = '-';
			ip++;
		}
		if (code[i] < 10){
			license[ip] = '0' + (char)code[i];
			continue;
		}
		if (code[i] < 36){
			license[ip] = 'a' + (char)code[i] - 10;
			continue;
		}
		license[ip] = 'A' + (char)code[i] - 36;
	}
}