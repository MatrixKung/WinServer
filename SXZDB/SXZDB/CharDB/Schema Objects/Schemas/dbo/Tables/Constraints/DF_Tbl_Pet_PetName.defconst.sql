﻿ALTER TABLE [dbo].[Tbl_Pet]
    ADD CONSTRAINT [DF_Tbl_Pet_PetName] DEFAULT ('') FOR [PetName];

