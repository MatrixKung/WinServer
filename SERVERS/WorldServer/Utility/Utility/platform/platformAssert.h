
#ifndef _PLATFORMASSERT_H_
#define _PLATFORMASSERT_H_

#include "base/types.h"

class PlatformAssert
{	
public:
	enum Type 
	{
		PA_WARNING	= 3,	//����ȼ����
		PA_ERROR	= 2,	//����ȼ����
		PA_FOCUS	= 1		//��������ȼ����
	};

	PlatformAssert();
	~PlatformAssert();	
	void Output(Type logType, const char* filename, U32 line, const char* msg);
	void Assert(Type logType, const char* filename, U32 line, const char* msg, bool isChoice=true);
	void Crash(Type logType, const char* msg);
};

extern PlatformAssert* g_pAssert;

#define AssertWarn(x,y) \
	{  if (((bool)(x))==(bool)0) \
		{g_pAssert->Output(PlatformAssert::PA_WARNING, __FILE__, __LINE__,  y);}}

#define AssertFatal(x, y) \
	{  if (((bool)(x))==(bool)0) \
		{g_pAssert->Output(PlatformAssert::PA_ERROR, __FILE__, __LINE__,  y);\
		 g_pAssert->Crash(PlatformAssert::PA_WARNING,y);}}

#define AssertRelease(x,y) \
	{  if (((bool)(x))==(bool)0) \
		{g_pAssert->Output(PlatformAssert::PA_ERROR, __FILE__, __LINE__,  y);\
		 g_pAssert->Crash(PlatformAssert::PA_ERROR,y);}}

#define AssertError(x, y, action) \
	{  if (((bool)(x))==(bool)0) \
		{g_pAssert->Output(PlatformAssert::PA_ERROR, __FILE__, __LINE__,  y);action;}}

//ע��ĿǰAssert����˵��
//----------------------------------------------------------------------------------------------
//                       DEBUG                          RELEASE
// AssertWarn		  LOG						      LOG
// AssertFatal        LOG+�ж�ִ��					  LOG
// AssertRelease      LOG+�ж�ִ�С�������������������LOG+�ж�ִ��
// AssertError        LOG+action					  LOG+action


//ע��ԭAssert����˵��
//----------------------------------------------------------------------------------------------
//                       DEBUG                          RELEASE
// AssertWarn		  LOG						      LOG
// AssertFatal        LOG+�ж�ִ��					  LOG
// AssertRelease      LOG+�ж�ִ�С�������������������LOG+�ж�ִ��
// AssertISV          LOG+�ж�ִ��                    LOG+�ж�ִ��
// Safe_AssertWarn    LOG+action					  LOG+action
// Safe_AssertFatal   LOG+�ж�ִ��+action			  LOG+action
// Safe_AssertRelease LOG+�ж�ִ��+action			  LOG+�ж�ִ��+action

extern const char* avar(const char *in_msg, ...);

#endif // _PLATFORM_ASSERT_H_

