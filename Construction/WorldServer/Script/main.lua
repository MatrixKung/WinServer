-----------------------------------------------------------------------------------------------------
-----������߻�ʹ�õ�lua�ű��ӿ�
-----------------------------------------------------------------------------------------------------
dofile("script/globalFunction.lua");
--[[
ע��һ�㣺���е���Щ��Ա����ֻ�ܲ��գ������޸ģ��޸�Ҫ���þ����c++���룬�ĵ��ڴ�����ݿ�
˵����Player player = getPlayer(nPlayerId);������ȡ��Ҷ���������Player,���������player�� nPlayerIdΪ���id
ȡ����������Ժ�Player�е�isNil()�����ж���������Ƿ���Ч ��
local Player player = getPlayer(50000001);
if(player:isNil() == true) then
	-- todo
end
CMissionDataPtr��CMissionDataΪ����data��CMissionDataPtr��ָ������͵ģ���nil�ж�ֱ���Ƿ�Ϊ�ա�CMissionData�Ƕ����͵ġ�����ȡ��Ա������ʽһ��
	��Ա������., ��Ա������:(stTimeSetPtr��stTimeSetһ��)
getMissionData(iMissionId);��ȡ����data������������CMissionDataPtr���͡����Ա���÷�������� AccountHandle�е� Export�еķ���
���磺  
//��ȡ700001�����data
local CMissionDataPtr pMissionData = getMissionData(700001);
	if(pMissionData ~= nil) then
		pMissionData:printAllEmlem();
end
]]--

function testAllMission(plyaerId)
	local Player player = getPlayer(50000001);
	if(player:isNil() == true) then
		for i =0, 511 do
			local stTimeSetPtr pTimer = player:getMission(i);
			if(pTimer ~= nil) then
				print(pTimer .id);
			end
		end
	end
end
