USE [SXZ1_ACTOR]
GO
/****** ����:  StoredProcedure [dbo].[Sp_DeletePlayerSocial]    �ű�����: 06/10/2013 15:33:03 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[Sp_DeletePlayerSocial]
@PlayerID INT
AS
BEGIN
	SET NOCOUNT ON
	SET XACT_ABORT ON
	BEGIN TRANSACTION DeletePlayerSocial
	DELETE FROM TBL_SOCIAL WHERE Player=@PlayerID
	UPDATE TBL_SOCIAL SET FriendValue=0 WHERE  target=@PlayerID
	COMMIT TRANSACTION DeletePlayerSocial
END