﻿ALTER TABLE [dbo].[Tbl_Player]
    ADD CONSTRAINT [DF_Tbl_Player_SwitchState] DEFAULT ((0)) FOR [SwitchState];

