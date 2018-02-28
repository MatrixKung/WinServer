﻿CREATE TABLE [dbo].[TBL_PURCHASE](
	[UID] varchar(33) NOT NULL,
	[Order] varchar(180)	CONSTRAINT [DF_TBL_PURCHASE_Order]	DEFAULT((''))	NOT NULL,
	[Status] int			CONSTRAINT [DF_TBL_PURCHASE_Status] DEFAULT((0))	NOT NULL,
	[CreateTime] datetime	CONSTRAINT [DF_TBL_PURCHASE_CreateTime] DEFAULT(getdate()) NOT NULL,
)