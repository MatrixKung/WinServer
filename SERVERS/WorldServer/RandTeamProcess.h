//#ifndef _RANDTEAMPROCESS_H_
//#define _RANDTEAMPROCESS_H_
//
//class RandTeam;
//
//class RandTeamProcess
//{
//public:
//	virtual ~RandTeamProcess(void) {}
//
//    virtual bool PreProcess(RandTeam* pRandTeam) {return true;}
//	virtual bool Process(RandTeam* pRandTeam) = 0;
//};
//
//class RandTeamFailProcess : public RandTeamProcess
//{
//public:
//	//method from RandTeamProcess
//	virtual bool Process(RandTeam* pRandTeam);
//};
//
////������,���븱��ȷ��
//class RandTeamEnterProcess : public RandTeamProcess
//{
//public:
//	//method from RandTeamProcess
//	virtual bool Process(RandTeam* pRandTeam);
//};
//
////��ӵȴ��б�
////��ʼ��ӻ����������Աʱ���ó��������
//class RandTeamWaitProcess : public RandTeamProcess
//{
//public:
//	RandTeamWaitProcess(void);
//
//	//method from RandTeamProcess
//	virtual bool Process(RandTeam* pRandTeam);
//protected:
//	bool __SelectPlayer(RandTeam* pRandTeam,U32 charType,U32 count);
//};
//
//#endif /*_RANDTEAMPROCESS_H_*/