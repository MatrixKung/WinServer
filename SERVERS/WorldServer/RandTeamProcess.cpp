//#include "RandTeamProcess.h"
//
//bool RandTeamFailProcess::Process(RandTeam* pRandTeam)
//{
//    if (0 == pRandTeam)
//		return false;
//
//    U32 memberCount = pRandTeam->GetCount();
//
//    if (0 == memberCount)
//    {
//        pRandTeam->SetProcess(0);
//    }
//    //else
//    //{
//    //    //���������Ŷ��б�
//    //    pRandTeam->SetProcess();
//    //}
//}
//
//bool RandTeamAckProcess::PreProcess(RandTeam* pRandTeam)
//{
//    if (0 == pRandTeam)
//        return false;
//
//    //���Ա����������ȷ��
//    char buf[256];
//    Base::BitStream SendPacket(buf,sizeof(buf));
//
//    stPacketHead* pHead = IPacket::BuildPacketHead(SendPacket,WZ_RANDTEAM_TEAMINVITE,0,SERVICE_ZONESERVER);
//
//    SendPacket.writeInt(pRandTeam->GetMapId(),Base::Bit32);
//    SendPacket.writeInt(pRandTeam->GetMapLevel(),Base::Bit32);
//
//    pHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);
//    pRandTeam->Broadcast(pHead,SendPacket);
//
//    return true;
//}
//
//bool RandTeamAckProcess::Process(RandTeam* pRandTeam)
//{
//	if (0 == pRandTeam)
//		return false;
//
//	U32 readyCount = 0;
//	
//	for (U32 i = 0; i < TEAM_MEMBER_MAX; ++i)
//	{
//		RandTeamPlayer& member = pRandTeam->GetMember(i);
//		
//		if (!member.IsValid())
//			continue;
//
//		//����ж�Ա�ܾ�,�����ʧ��
//		if (RandTeamPlayer::PIS_REJECT == member.GetStatus())
//		{
//			pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetFailProcess());
//			return true;
//		}
//
//		//���Ա����ȷ������
//		if (RandTeamPlayer::PIS_NORMAL == member.GetStatus())
//		{
//			
//		}
//
//		if (RandTeamPlayer::PIS_ACCEPT == member.GetStatus())
//		{
//			readyCount++;
//		}
//	}
//
//	//������Ҷ��Ѿ�ȷ��
//	if (readyCount == pRandTeam->GetCount())
//	{
//		//�жϽ�ɫ�Ƿ�ֲ���ȷ
//		U32 nDefenders = pRandTeam->GetCharCount(COPYMAP_ACTOR_DEFENCE);
//		U32 nHealers   = pRandTeam->GetCharCount(COPYMAP_ACTOR_DOCTOR);
//		
//		if (nDefenders < pRandTeam->GetCharMaxCount(COPYMAP_ACTOR_DEFENCE) || nHealers < pRandTeam->GetCharMaxCount(COPYMAP_ACTOR_DOCTOR))
//		{
//			//ȱ�ٱ�Ҫ�ķ����ߺ�������
//			
//
//			pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetFailProcess());
//		}
//		else
//		{
//			
//			//���뵽�Ŷ��б�
//			pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetWaitProcess(),3600);
//		}
//
//		return true;
//	}
//
//	//��ʱ,���ʧ��
//	if (pRandTeam->IsExpire())
//	{
//		pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetFailProcess(),60);
//	}
//
//	return true;
//}
//
//bool RandTeamWaitProcess::Process(RandTeam* pRandTeam)
//{
//	if (0 == pRandTeam)
//		return false;
//
//	RandTeamPlayer& starter = pRandTeam->GetMember(0);
//
//	if (!starter.IsValid())
//		return false;
//	
//	//ɸѡ��ɫ
//	for (U32 i = 0; i < RandTeamPlayer::CT_COUNT; ++i)
//	{
//		U32 curCount = pRandTeam->GetCharCount(i);
//
//		if (pRandTeam->GetCharMaxCount(i) > curCount)
//		{
//			__SelectPlayer(pRandTeam,i,pRandTeam->GetCharMaxCount(i) - curCount);
//		}
//	}
//
//	//�ж��Ƿ������������
//	if (pRandTeam->IsTeamFull())
//	{
//		//�������δ����,��������ҷ��ͽ���ȷ��
//		if (!pRandTeam->IsCreated())
//			pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetEnterProcess(),60);
//		else
//			pRandTeam->SetProcess(SERVER->GetRandTeamMgr()->GetInviteProcess(),60);
//	}
//	
//	return true;
//}
