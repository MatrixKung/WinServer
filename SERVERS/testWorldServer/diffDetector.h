//
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <set>
#include <map>


struct Diff//�Ƚ�֮����Ҫ���µ�����
{
	std::vector<int> m_vecNeedUpdate;//��Ҫ���µ�index ��ָ�������е�Index
	std::vector<int> m_vecNeedInsert;//��Ҫ��ӵ�index ��ָ�������е�Index
	std::vector<int> m_vecNeedDelete;//��Ҫɾ����index ��ָ�������е�Index
};

template<typename ElementStruct,int MaxCount,typename keytype>
class DiffDetector
{
public:
	typedef std::map<keytype,int> MapKey2Index;
public:
	static bool IsMemorySame(ElementStruct& a, ElementStruct& b) //�ж������ṹ�Ƿ���ȫ���
	{
		return (memcmp(&a,&b,sizeof(ElementStruct) == 0));
	}
	//�Ƚ������ṹ���飬��pKey�ֶ�Ϊ0��Ϊ�ս�����õ���Щ��¼��Ҫ����/ɾ��/���
	static Diff GetDiff(ElementStruct (&oldData)[MaxCount], 
					    ElementStruct (&newData)[MaxCount],
						keytype ElementStruct::*pKey)
	{
		Diff res;
		MapKey2Index t_MapOldKey2Index;
		MapKey2Index t_MapNewKey2Index;
		for (int i=0; i<MaxCount; i++)
		{
			if (oldData[i].*pKey ==0 ) break; //0���ս��
			t_MapOldKey2Index.insert(std::make_pair(oldData[i].*pKey,i));
			
		}
		MapKey2Index::iterator oldset_end = t_MapOldKey2Index.end();
		
		for (int i=0; i<MaxCount; i++)//newkeyset��û�г�����oldkeyset�е���Ҫinsert
		{
			if (newData[i].*pKey==0) break; //�ս��
			if (t_MapOldKey2Index.find(newData[i].*pKey) == oldset_end)
			{
				res.m_vecNeedInsert.push_back(i);//��Ҫinsert
			}			
			t_MapNewKey2Index.insert(std::make_pair(newData[i].*pKey,i));			
		}

		MapKey2Index::iterator newset_end = t_MapNewKey2Index.end();

		for (int i=0; i<MaxCount; i++)//oldkeyset��û�г�����newkeyset�е���Ҫdelete
		{
			if (oldData[i].*pKey==0) break; //�ս��
			if (t_MapNewKey2Index.find(oldData[i].*pKey) == newset_end)
			{
				res.m_vecNeedDelete.push_back(i);//��Ҫɾ��
			}			
		}

		MapKey2Index::iterator itNewMap = t_MapNewKey2Index.begin();
		MapKey2Index::iterator itNewMapEnd = t_MapNewKey2Index.end();
		MapKey2Index::iterator itOldMapEnd = t_MapOldKey2Index.end();
		for (; itNewMap!= itNewMapEnd; itNewMap++)
		{
			MapKey2Index::iterator itOldMap = t_MapOldKey2Index.find(itNewMap->first);
			if (itOldMap != itOldMapEnd)//����newMap��ͬʱҲ������OldMap ������Ҫ����
			{
				if (!IsMemorySame(newData[itNewMap->second], oldData[itOldMap->second]))//�ж������ڴ��Ƿ����
				{
					res.m_vecNeedUpdate.push_back(itNewMap->second);//��Ҫ����
				}
			}
		}


		return res;

		
	}
};