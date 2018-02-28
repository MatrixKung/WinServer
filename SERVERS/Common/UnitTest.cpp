/////////////////////////////////////////////////////////////////
/// @file UnitTest.cpp
///
/// ���ڵ�Ԫ����.
///
/// ���ڵ�Ԫ����.
///
///	@���� LivenHotch. 
///
/// @����  06/03/2010
//  -------------------------------------------------------------
//  �޸ļ�¼.
//  06/03/2010    �ļ�����
//
//  Copyright (C) 2010 - All Rights Reserved
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Method:    UnitTestMain
/// FullName:  UnitTestMain
/// Access:    public 
/// Returns:   void
/// Brief:	   ��Ԫ�������
/// Qualifier:
/////////////////////////////////////////////////////////////////
#include "UnitTest.h"

void UnitTestMain()
{
#ifdef DEBUG
	__security_init_cookie();
	_heap_init(1);
	_mtinit();
	_ioinit();
	_cinit( 1 );
	_cexit();
#endif
}


/////////////////////////////////////////////////////////////////
/// Method:    UnitTest
/// FullName:  UnitTest::UnitTest
/// Access:    public 
/// Returns:   
/// Brief:	   ��Ԫ���Թ��캯��
/// Qualifier:
/// Parameter: const char * TestName
/// Parameter: UnitTestFunction function
/////////////////////////////////////////////////////////////////
UnitTest::UnitTest( const char* TestName, UnitTestFunction function )
{
	printf( "\n\rUNIT TEST: %s\n\r", TestName );

	try
	{
		function();

		printf( "\n\rTEST SUCCESSED !\n\r" );
	}
	catch( UnitTestException e )
	{
		printf( "\n\rTEST FAIL: %s\n\r", e.getError() );
	}
	catch( ... )
	{
		printf( "\n\rTEST FAIL: UNKNOWN\n\r" );
	}

}

/////////////////////////////////////////////////////////////////
/// Method:    ~UnitTest
/// FullName:  UnitTest::~UnitTest
/// Access:    public 
/// Returns:   
/// Brief:	   ��Ԫ������������
/// Qualifier:
/////////////////////////////////////////////////////////////////
UnitTest::~UnitTest()
{

}

/////////////////////////////////////////////////////////////////
/// Method:    UnitTestException
/// FullName:  UnitTestException::UnitTestException
/// Access:    public 
/// Returns:   
/// Brief:	   ��Ԫ�����쳣���캯��
/// Qualifier:
/// Parameter: const char * error
/////////////////////////////////////////////////////////////////
UnitTestException::UnitTestException( const char* error )
{
	mError = error;
}

/////////////////////////////////////////////////////////////////
/// Method:    getError
/// FullName:  UnitTestException::getError
/// Access:    public 
/// Returns:   const char*
/// Brief:	   ��ȡ��Ԫ�����쳣����
/// Qualifier: const
/////////////////////////////////////////////////////////////////
const char* UnitTestException::getError() const
{
	return mError.c_str();
}

