﻿ALTER TABLE [dbo].[Tbl_Price_Version]
    ADD CONSTRAINT [DF_Tbl_Price_Version_LastTime] DEFAULT (getdate()) FOR [LastTime];

