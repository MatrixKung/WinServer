﻿ALTER TABLE [dbo].[Tbl_BankItem]
    ADD CONSTRAINT [DF_Tbl_BankItem_IsTradeActive] DEFAULT ((0)) FOR [IsTradeActive];

