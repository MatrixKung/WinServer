#pragma once

#include <stdio.h>
#include <vector>
#include "DatBuilder/DatBuilder.h"

class CDataFile
{
public:
	int  RecordNum;  //��¼����
	int  ColumNum;   //������

	CDataFile();
	~CDataFile();
	//
	bool readDataFile   (const char* name);    //��ȡdata�ļ�
	bool GetData        (RData&);              //�õ����������
	//
	void ReadDataInit   ();
	void ReadDataClose  ();
private:
	FILE* pfdata;
	int       readstep;   //���ƶ����ܵ�����
	//

public:
	std::vector<U8> m_DataTypes;
	int	m_CurrentColumnIndex;
};