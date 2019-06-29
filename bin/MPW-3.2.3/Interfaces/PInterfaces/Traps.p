{
Created: Saturday, September 16, 1989 at 3:14 PM
	Traps.p
	Pascal Interface to the Macintosh Libraries

	Copyright Apple Computer, Inc.	1985-1990
	All rights reserved
}


{$IFC UNDEFINED UsingIncludes}
{$SETC UsingIncludes := 0}
{$ENDC}

{$IFC NOT UsingIncludes}
	UNIT Traps;
	INTERFACE
{$ENDC}

{$IFC UNDEFINED UsingTraps}
{$SETC UsingTraps := 1}

CONST

{  Control Manager  }

_DisposControl					 = $A955;
_DragControl					 = $A967;
_Draw1Control					 = $A96D;
_DrawControls					 = $A969;
_FindControl					 = $A96C;
_GetAuxCtl						 = $AA44;
_GetCRefCon 					 = $A95A;
_GetCTitle						 = $A95E;
_GetCtlAction					 = $A96A;
_GetCtlValue					 = $A960;
_GetCVariant					 = $A809;
_GetMaxCtl						 = $A962;
_GetMinCtl						 = $A961;
_GetNewControl					 = $A9BE;
_HideControl					 = $A958;
_HiliteControl					 = $A95D;
_KillControls					 = $A956;
_MoveControl					 = $A959;
_NewControl 					 = $A954;
_SetCRefCon 					 = $A95B;
_SetCTitle						 = $A95F;
_SetCtlAction					 = $A96B;
_SetCtlColor					 = $AA43;
_SetCtlValue					 = $A963;
_SetMaxCtl						 = $A965;
_SetMinCtl						 = $A964;
_ShowControl					 = $A957;
_SizeControl					 = $A95C;
_TestControl					 = $A966;
_TrackControl					 = $A968;
_UpdtControl					 = $A953;

{  Desk Manager  }

_CloseDeskAcc					 = $A9B7;
_OpenDeskAcc					 = $A9B6;
_SysEdit						 = $A9C2;
_SystemClick					 = $A9B3;
_SystemEvent					 = $A9B2;
_SystemMenu 					 = $A9B5;
_SystemTask 					 = $A9B4;

{  Apple Desktop Bus  }

_ADBReInit						 = $A07B;

{  Dialog Manager  }

_NewCDialog 					 = $AA4B;
_Alert							 = $A985;
_CautionAlert					 = $A988;
_CloseDialog					 = $A982;
_CouldAlert 					 = $A989;
_CouldDialog					 = $A979;
_DialogSelect					 = $A980;
_DisposDialog					 = $A983;
_DrawDialog 					 = $A981;
_ErrorSound 					 = $A98C;
_FindDItem						 = $A984;
_FreeAlert						 = $A98A;
_FreeDialog 					 = $A97A;
_GetDItem						 = $A98D;
_GetIText						 = $A990;
_GetNewDialog					 = $A97C;
_HideDItem						 = $A827;
_InitDialogs					 = $A97B;
_IsDialogEvent					 = $A97F;
_ModalDialog					 = $A991;
_NewDialog						 = $A97D;
_NoteAlert						 = $A987;
_ParamText						 = $A98B;
_SelIText						 = $A97E;
_SetDItem						 = $A98E;
_SetIText						 = $A98F;
_ShowDItem						 = $A828;
_StopAlert						 = $A986;
_UpdtDialog 					 = $A978;
_StdOpcodeProc					 = $ABF8;

{  Events   }

_Button 						 = $A974;
_EventAvail 					 = $A971;
_GetKeys						 = $A976;
_GetMouse						 = $A972;
_GetNextEvent					 = $A970;
_TickCount						 = $A975;
_WaitMouseUp					 = $A977;
_WaitNextEvent					 = $A860;

{  FixMath   }

_FixATan2						 = $A818;

{  Fonts 	}

_SetFractEnable 				 = $A814;
_FMSwapFont 					 = $A901;
_FontMetrics					 = $A835;
_GetFName						 = $A8FF;
_GetFNum						 = $A900;
_InitFonts						 = $A8FE;
_RealFont						 = $A902;
_SetFontLock					 = $A903;
_SetFScaleDisable				 = $A834;

{  Menus 	}

_DelMCEntries					 = $AA60;
_DispMCInfo 					 = $AA63;
_GetMCEntry 					 = $AA64;
_GetMCInfo						 = $AA61;
_MenuChoice 					 = $AA66;
_SetMCEntries					 = $AA65;
_SetMCInfo						 = $AA62;
_AddResMenu 					 = $A94D;
_AppendMenu 					 = $A933;
_CalcMenuSize					 = $A948;
_CheckItem						 = $A945;
_ClearMenuBar					 = $A934;
_CountMItems					 = $A950;
_DeleteMenu 					 = $A936;
_DelMenuItem					 = $A952;
_DisableItem					 = $A93A;
_DisposMenu 					 = $A932;
_DrawMenuBar					 = $A937;
_InvalMenuBar					 = $A81D;
_EnableItem 					 = $A939;
_FlashMenuBar					 = $A94C;
_GetItem						 = $A946;
_GetItemCmd 					 = $A84E;
_GetItmIcon 					 = $A93F;
_GetItmMark 					 = $A943;
_GetMenuBar 					 = $A93B;
_GetMHandle 					 = $A949;
_GetNewMBar 					 = $A9C0;
_GetRMenu						 = $A9BF;
_HiliteMenu 					 = $A938;
_InitMenus						 = $A930;
_InitProcMenu					 = $A808;
_InsertMenu 					 = $A935;
_InsertResMenu					 = $A951;
_InsMenuItem					 = $A826;
_MenuKey						 = $A93E;
_MenuSelect 					 = $A93D;
_NewMenu						 = $A931;
_PopUpMenuSelect				 = $A80B;
_SetItem						 = $A947;
_SetItemCmd 					 = $A84F;
_SetItmIcon 					 = $A940;
_SetItmMark 					 = $A944;
_SetItmStyle					 = $A942;
_SetMenuBar 					 = $A93C;
_SetMFlash						 = $A94A;

{  OSUtils   }

_KeyTrans						 = $A9C3;
_SysBeep						 = $A9C8;
_Unimplemented					 = $A89F;
_HWPriv 						 = $A198;
_InitDogCow 					 = $A89F;
_EnableDogCow					 = $A89F;
_DisableDogCow					 = $A89F;
_Moof							 = $A89F;

{  Packages   }

_InitAllPacks					 = $A9E6;
_InitPack						 = $A9E5;

{  Quickdraw 	}

_AddComp						 = $AA3B;
_AddPt							 = $A87E;
_AddSearch						 = $AA3A;
_AllocCursor					 = $AA1D;
_BackColor						 = $A863;
_BackPat						 = $A87C;
_BackPixPat 					 = $AA0B;
_CalcMask						 = $A838;
_CharExtra						 = $AA23;
_CharWidth						 = $A88D;
_ClipRect						 = $A87B;
_CloseCport 					 = $A87D;
_ClosePgon						 = $A8CC;
_ClosePicture					 = $A8F4;
_ClosePort						 = $A87D;
_CloseRgn						 = $A8DB;
_Color2Index					 = $AA33;
_ColorBit						 = $A864;
_CopyBits						 = $A8EC;
_CopyMask						 = $A817;
_CopyPixMap 					 = $AA05;
_CopyPixPat 					 = $AA09;
_CopyRgn						 = $A8DC;
_DelComp						 = $AA4D;
_DelSearch						 = $AA4C;
_DeviceLoop						 = $ABCA;
_DiffRgn						 = $A8E6;
_DisposCCursor					 = $AA26;
_DisposCIcon					 = $AA25;
_DisposCTable					 = $AA24;
_DisposGDevice					 = $AA30;
_DisposPixMap					 = $AA04;
_DisposPixPat					 = $AA08;
_DisposRgn						 = $A8D9;
_DrawChar						 = $A883;
_DrawPicture					 = $A8F6;
_DrawString 					 = $A884;
_DrawText						 = $A885;
_EmptyRect						 = $A8AE;
_EmptyRgn						 = $A8E2;
_EqualPt						 = $A881;
_EqualRect						 = $A8A6;
_EqualRgn						 = $A8E3;
_EraseArc						 = $A8C0;
_EraseOval						 = $A8B9;
_ErasePoly						 = $A8C8;
_EraseRect						 = $A8A3;
_EraseRgn						 = $A8D4;
_EraseRoundRect 				 = $A8B2;
_FillArc						 = $A8C2;
_FillCArc						 = $AA11;
_FillCOval						 = $AA0F;
_FillCPoly						 = $AA13;
_FillCRect						 = $AA0E;
_FillCRgn						 = $AA12;
_FillCRoundRect 				 = $AA10;
_FillOval						 = $A8BB;
_FillPoly						 = $A8CA;
_FillRect						 = $A8A5;
_FillRgn						 = $A8D6;
_BitMapToRegion 				 = $A8D7;
_FillRoundRect					 = $A8B4;
_ForeColor						 = $A862;
_FrameArc						 = $A8BE;
_FrameOval						 = $A8B7;
_FramePoly						 = $A8C6;
_FrameRect						 = $A8A1;
_FrameRgn						 = $A8D2;
_FrameRoundRect 				 = $A8B0;
_GetBackColor					 = $AA1A;
_GetCCursor 					 = $AA1B;
_GetCIcon						 = $AA1E;
_GetClip						 = $A87A;
_GetCPixel						 = $AA17;
_GetCTable						 = $AA18;
_GetCTSeed						 = $AA28;
_GetDeviceList					 = $AA29;
_GetFontInfo					 = $A88B;
_GetForeColor					 = $AA19;
_GetGDevice 					 = $AA32;
_GetMainDevice					 = $AA2A;
_GetMaxDevice					 = $AA27;
_GetNextDevice					 = $AA2B;
_GetPen 						 = $A89A;
_GetPenState					 = $A898;
_GetPixel						 = $A865;
_GetPixPat						 = $AA0C;
_GetPort						 = $A874;
_GetSubTable					 = $AA37;
_GlobalToLocal					 = $A871;
_GrafDevice 					 = $A872;
_HideCursor 					 = $A852;
_HidePen						 = $A896;
_HiliteColor					 = $AA22;
_Index2Color					 = $AA34;
_InitCport						 = $AA01;
_InitCursor 					 = $A850;
_InitGDevice					 = $AA2E;
_InitGraf						 = $A86E;
_InitPort						 = $A86D;
_InSetRect						 = $A8A9;
_InSetRgn						 = $A8E1;
_InverRect						 = $A8A4;
_InverRgn						 = $A8D5;
_InverRoundRect 				 = $A8B3;
_InvertArc						 = $A8C1;
_InvertColor					 = $AA35;
_InvertOval 					 = $A8BA;
_InvertPoly 					 = $A8C9;
_KillPicture					 = $A8F5;
_KillPoly						 = $A8CD;
_Line							 = $A892;
_LineTo 						 = $A891;
_LocalToGlobal					 = $A870;
_MakeITable 					 = $AA39;
_MakeRGBPat 					 = $AA0D;
_MapPoly						 = $A8FC;
_MapPt							 = $A8F9;
_MapRect						 = $A8FA;
_MapRgn 						 = $A8FB;
_MeasureText					 = $A837;
_Move							 = $A894;
_MovePortTo 					 = $A877;
_MoveTo 						 = $A893;
_NewGDevice 					 = $AA2F;
_NewPixMap						 = $AA03;
_NewPixPat						 = $AA07;
_NewRgn 						 = $A8D8;
_ObscureCursor					 = $A856;
_OffSetPoly 					 = $A8CE;
_OffSetRect 					 = $A8A8;
_OfSetRgn						 = $A8E0;
_OpColor						 = $AA21;
_OpenCport						 = $AA00;
_OpenPicture					 = $A8F3;
_OpenPoly						 = $A8CB;
_OpenPort						 = $A86F;
_OpenRgn						 = $A8DA;
_PaintArc						 = $A8BF;
_PaintOval						 = $A8B8;
_PaintPoly						 = $A8C7;
_PaintRect						 = $A8A2;
_PaintRgn						 = $A8D3;
_PaintRoundRect 				 = $A8B1;
_PenMode						 = $A89C;
_PenNormal						 = $A89E;
_PenPat 						 = $A89D;
_PenPixPat						 = $AA0A;
_PenSize						 = $A89B;
_PicComment 					 = $A8F2;
_PlotCIcon						 = $AA1F;
_PortSize						 = $A876;
_ProtectEntry					 = $AA3D;
_Pt2Rect						 = $A8AC;
_PtInRect						 = $A8AD;
_PtInRgn						 = $A8E8;
_PtToAngle						 = $A8C3;
_QDError						 = $AA40;
_Random 						 = $A861;
_RealColor						 = $AA36;
_RectInRgn						 = $A8E9;
_RectRgn						 = $A8DF;
_ReserveEntry					 = $AA3E;
_RestoreEntries 				 = $AA4A;
_RGBBackColor					 = $AA15;
_RGBForeColor					 = $AA14;
_SaveEntries					 = $AA49;
_ScalePt						 = $A8F8;
_ScrollRect 					 = $A8EF;
_SectRect						 = $A8AA;
_SectRgn						 = $A8E4;
_SeedCFill						 = $AA50;
_SeedFill						 = $A839;
_SetCCursor 					 = $AA1C;
_SetClientID					 = $AA3C;
_SetClip						 = $A879;
_SetCPixel						 = $AA16;
_SetPortPix 					 = $AA06;
_SetCursor						 = $A851;
_SetDeviceAttribute 			 = $AA2D;
_SetEmptyRgn					 = $A8DD;
_SetEntries 					 = $AA3F;
_SetGDevice 					 = $AA31;
_SetOrigin						 = $A878;
_SetPBits						 = $A875;
_SetPenState					 = $A899;
_SetPort						 = $A873;
_SetPt							 = $A880;
_SetRecRgn						 = $A8DE;
_SetRect						 = $A8A7;
_SetStdCProcs					 = $AA4E;
_SetStdProcs					 = $A8EA;
_ShowCursor 					 = $A853;
_ShowPen						 = $A897;
_SpaceExtra 					 = $A88E;
_StdArc 						 = $A8BD;
_StdBits						 = $A8EB;
_StdComment 					 = $A8F1;
_StdGetPic						 = $A8EE;
_StdLine						 = $A890;
_StdOval						 = $A8B6;
_StdPoly						 = $A8C5;
_StdPutPic						 = $A8F0;
_StdRect						 = $A8A0;
_StdRgn 						 = $A8D1;
_StdRRect						 = $A8AF;
_StdText						 = $A882;
_StdTxMeas						 = $A8ED;
_StringWidth					 = $A88C;
_StuffHex						 = $A866;
_SubPt							 = $A87F;
_TestDeviceAttribute			 = $AA2C;
_TextFace						 = $A888;
_TextFont						 = $A887;
_TextMode						 = $A889;
_TextSize						 = $A88A;
_TextWidth						 = $A886;
_UnionRect						 = $A8AB;
_UnionRgn						 = $A8E5;
_XOrRgn 						 = $A8E7;
_CalcCMask						 = $AA4F;
_GetMaskTable					 = $A836;
_UpdatePixMap					 = $AA38;

{  Resources 	}

_AddResource					 = $A9AB;
_ChangedResource				 = $A9AA;
_CloseResFile					 = $A99A;
_Count1Resources				 = $A80D;
_Count1Types					 = $A81C;
_CountResources 				 = $A99C;
_CountTypes 					 = $A99E;
_CreateResFile					 = $A9B1;
_CurResFile 					 = $A994;
_DetachResource 				 = $A992;
_Get1IxResource 				 = $A80E;
_Get1IxType 					 = $A80F;
_Get1NamedResource				 = $A820;
_Get1Resource					 = $A81F;
_GetIndResource 				 = $A99D;
_GetIndType 					 = $A99F;
_GetNamedResource				 = $A9A1;
_GetResAttrs					 = $A9A6;
_GetResFileAttrs				 = $A9F6;
_GetResInfo 					 = $A9A8;
_GetResource					 = $A9A0;
_HCreateResFile					 = $A81B;
_HOpenResFile					 = $A81A;
_HomeResFile					 = $A9A4;
_InitResources					 = $A995;
_LoadResource					 = $A9A2;
_MaxSizeRsrc					 = $A821;
_OpenResFile					 = $A997;
_OpenRFPerm 					 = $A9C4;
_ReleaseResource				 = $A9A3;
_ResError						 = $A9AF;
_RGetResource					 = $A80C;
_RmveResource					 = $A9AD;
_RsrcMapEntry					 = $A9C5;
_RsrcZoneInit					 = $A996;
_SetResAttrs					 = $A9A7;
_SetResFileAttrs				 = $A9F7;
_SetResInfo 					 = $A9A9;
_SetResLoad 					 = $A99B;
_SetResPurge					 = $A993;
_SizeRsrc						 = $A9A5;
_Unique1ID						 = $A810;
_UniqueID						 = $A9C1;
_UpdateResFile					 = $A999;
_UseResFile 					 = $A998;
_WriteResource					 = $A9B0;
_Pack8							 = $A816;
_Pack9							 = $A82B;
_Pack10 						 = $A82C;
_Pack11 						 = $A82D;
_Pack12 						 = $A82E;
_Pack13 						 = $A82F;
_Pack14 						 = $A830;
_Pack15 						 = $A831;
_ScrnBitMap 					 = $A833;
_DragTheRgn 					 = $A926;
_GetItmStyle					 = $A941;
_PlotIcon						 = $A94B;
_Dequeue						 = $A96E;
_Enqueue						 = $A96F;
_StillDown						 = $A973;
_AddReference					 = $A9AC;
_RmveReference					 = $A9AE;
_Secs2Date						 = $A9C6;
_Date2Secs						 = $A9C7;
_SysError						 = $A9C9;
_HandToHand 					 = $A9E1;
_PtrToXHand 					 = $A9E2;
_PtrToHand						 = $A9E3;
_HandAndHand					 = $A9E4;
_Pack0							 = $A9E7;
_Pack1							 = $A9E8;
_Pack2							 = $A9E9;
_Pack3							 = $A9EA;
_FP68K							 = $A9EB;
_Pack4							 = $A9EB;
_Elems68K						 = $A9EC;
_Pack5							 = $A9EC;
_Pack6							 = $A9ED;
_DECSTR68K						 = $A9EE;
_Pack7							 = $A9EE;
_PtrAndHand 					 = $A9EF;
_LoadSeg						 = $A9F0;
_Launch 						 = $A9F2;
_Chain							 = $A9F3;
_MethodDispatch 				 = $A9F8;
_Debugger						 = $A9FF;
_DebugStr						 = $ABFF;

{  Scrap Manager  }

_GetScrap						 = $A9FD;
_InfoScrap						 = $A9F9;
_LodeScrap						 = $A9FB;
_LoadScrap						 = $A9FB;
_PutScrap						 = $A9FE;
_UnlodeScrap					 = $A9FA;
_UnloadScrap					 = $A9FA;
_ZeroScrap						 = $A9FC;

{  SegLoad   }

_ExitToShell					 = $A9F4;
_GetAppParms					 = $A9F5;
_UnLoadSeg						 = $A9F1;

{  ShutDown   }

_ShutDown						 = $A895;

{  TextEdit   }

_TEActivate 					 = $A9D8;
_TEAutoView 					 = $A813;
_TECalText						 = $A9D0;
_TEClick						 = $A9D4;
_TECopy 						 = $A9D5;
_TECut							 = $A9D6;
_TEDeactivate					 = $A9D9;
_TEDelete						 = $A9D7;
_TEDispose						 = $A9CD;
_TEGetOffset					 = $A83C;
_TEGetText						 = $A9CB;
_TEIdle 						 = $A9DA;
_TEInit 						 = $A9CC;
_TEInsert						 = $A9DE;
_TEKey							 = $A9DC;
_TENew							 = $A9D2;
_TEPaste						 = $A9DB;
_TEPinScroll					 = $A812;
_TEScroll						 = $A9DD;
_TESelView						 = $A811;
_TESetJust						 = $A9DF;
_TESetSelect					 = $A9D1;
_TESetText						 = $A9CF;
_TEStyleNew 					 = $A83E;
_TEUpdate						 = $A9D3;
_TextBox						 = $A9CE;

{  ToolUtils 	}

_UnpackBits 					 = $A8D0;
_AngleFromSlope 				 = $A8C4;
_BitAnd 						 = $A858;
_BitClr 						 = $A85F;
_BitNot 						 = $A85A;
_BitOr							 = $A85B;
_BitSet 						 = $A85E;
_BitShift						 = $A85C;
_BitTst 						 = $A85D;
_BitXOr 						 = $A859;
_DeltaPoint 					 = $A94F;
_FixMul 						 = $A868;
_FixRatio						 = $A869;
_FixRound						 = $A86C;
_GetCursor						 = $A9B9;
_GetIcon						 = $A9BB;
_GetPattern 					 = $A9B8;
_GetPicture 					 = $A9BC;
_GetString						 = $A9BA;
_HiWord 						 = $A86A;
_LongMul						 = $A867;
_LoWord 						 = $A86B;
_Munger 						 = $A9E0;
_NewString						 = $A906;
_PackBits						 = $A8CF;
_SetString						 = $A907;
_ShieldCursor					 = $A855;
_SlopeFromAngle 				 = $A8BC;
_XMunger						 = $A819;
_WriteParam 					 = $A038;
_Open							 = $A000;
 {
 _______________________________________________________________________

  Core routine system 

 _______________________________________________________________________


  First we have the I/O core routines. These are also used by
  the file system.

}
_Close							 = $A001;
_Read							 = $A002;
_Write							 = $A003;
_Control						 = $A004;
_Status 						 = $A005;
_KillIO 						 = $A006;
_GetVolInfo 					 = $A007;
 {other file system calls go here
}
_Create 						 = $A008;
_Delete 						 = $A009;
_OpenRF 						 = $A00A;
_ReName 						 = $A00B;
_GetFileInfo					 = $A00C;
_SetFileInfo					 = $A00D;
_UnMountVol 					 = $A00E;
_MountVol						 = $A00F;
_Allocate						 = $A010;
_GetEOF 						 = $A011;
_SetEOF 						 = $A012;
_FlushVol						 = $A013;
_GetVol 						 = $A014;
_SetVol 						 = $A015;
_FInitQueue 					 = $A016;
_Eject							 = $A017;
_GetFPos						 = $A018;
_HSetVol						 = $A215;
_HGetVol						 = $A214;
_HOpen							 = $A200;
_HGetVInfo						 = $A207;
_HCreate						 = $A208;
_HDelete						 = $A209;
_HOpenRF						 = $A20A;
_HRename						 = $A20B;
_HGetFileInfo					 = $A20C;
_HSetFileInfo					 = $A20D;
_AllocContig					 = $A210;
_HSetFLock						 = $A241;
_HRstFLock						 = $A242;

{ Memory Manager }

_InitZone						 = $A019;
_GetZone						 = $A11A;
_SetZone						 = $A01B;
_FreeMem						 = $A01C;
_MaxMem 						 = $A11D;
_NewPtr 						 = $A11E;
_DisposPtr						 = $A01F;
_SetPtrSize 					 = $A020;
_GetPtrSize 					 = $A021;
_NewHandle						 = $A122;
_DisposHandle					 = $A023;
_SetHandleSize					 = $A024;
_GetHandleSize					 = $A025;
_HandleZone 					 = $A126;
_ReAllocHandle					 = $A027;
_RecoverHandle					 = $A128;
_HLock							 = $A029;
_HUnlock						 = $A02A;
_EmptyHandle					 = $A02B;
_InitApplZone					 = $A02C;
_SetApplLimit					 = $A02D;
_BlockMove						 = $A02E;
_MemoryDispatch					 = $A05C;
_MemoryDispatchA0Result			 = $A15C;
_DeferUserFn					 = $A08F;

{ OS Event Manager }

_PostEvent						 = $A02F;
_PPostEvent 					 = $A12F;
_OSEventAvail					 = $A030;
_GetOSEvent 					 = $A031;
_FlushEvents					 = $A032;
_VInstall						 = $A033;

{ utility core }

_VRemove						 = $A034;
_OffLine						 = $A035;
_MoreMasters					 = $A036;
_ReadDateTime					 = $A039;
_SetDateTime					 = $A03A;
_Delay							 = $A03B;
_CmpString						 = $A03C;
_DrvrInstall					 = $A03D;
_DrvrRemove 					 = $A03E;
_InitUtil						 = $A03F;
_ResrvMem						 = $A040;
_SetFilLock 					 = $A041;
_RstFilLock 					 = $A042;
_SetFilType 					 = $A043;
_SetFPos						 = $A044;
_FlushFile						 = $A045;
_GetTrapAddress 				 = $A146;
_SetTrapAddress 				 = $A047;
_PtrZone						 = $A148;
_HPurge 						 = $A049;
_HNoPurge						 = $A04A;
_SetGrowZone					 = $A04B;
_CompactMem 					 = $A04C;
_PurgeMem						 = $A04D;
_AddDrive						 = $A04E;
_RDrvrInstall					 = $A04F;
_UprString						 = $A054;
_LwrString						 = $A056;
_SetApplBase					 = $A057;
_OSDispatch 					 = $A88F;
_RelString						 = $A050;
 { new 128K ROM additions
}
_ReadXPRam						 = $A051;
_InsTime						 = $A058;
_RmvTime						 = $A059;
_PrimeTime						 = $A05A;
_MaxBlock						 = $A061;
_PurgeSpace 					 = $A162;
_MaxApplZone					 = $A063;
_MoveHHi						 = $A064;
_StackSpace 					 = $A065;
_NewEmptyHandle 				 = $A166;
_HSetRBit						 = $A067;
_HClrRBit						 = $A068;
_HGetState						 = $A069;
_HSetState						 = $A06A;
_InitFS 						 = $A06C;
_InitEvents 					 = $A06D;
_StripAddress					 = $A055;
 { end of System 

  new 256K ROM 
}
_SetAppBase 					 = $A057;
_SwapMMUMode					 = $A05D;
_SlotVInstall					 = $A06F;
_SlotVRemove					 = $A070;
_AttachVBL						 = $A071;
_DoVBLTask						 = $A072;
_SIntInstall					 = $A075;
_SIntRemove 					 = $A076;
_CountADBs						 = $A077;
_GetIndADB						 = $A078;
_GetADBInfo 					 = $A079;
_SetADBInfo 					 = $A07A;
_ADBOp							 = $A07C;
_GetDefaultStartup				 = $A07D;
_SetDefaultStartup				 = $A07E;
_InternalWait					 = $A07F;
_GetVideoDefault				 = $A080;
_SetVideoDefault				 = $A081;
_DTInstall						 = $A082;
_SetOSDefault					 = $A083;
_GetOSDefault					 = $A084;
_Sleep							 = $A08A;
_CommToolboxDispatch			 = $A08B;
_SysEnvirons					 = $A090;
_GestaltDispatch				 = $A0AD;
_InitPalettes					 = $AA90;
 {	Palette Manager, transplanted from PaletteEqu.a
}
_NewPalette 					 = $AA91;
_GetNewPalette					 = $AA92;
_DisposePalette 				 = $AA93;
_ActivatePalette				 = $AA94;
_SetPalette 					 = $AA95;
_NSetPalette					 = $AA95;
_GetPalette 					 = $AA96;
_PmForeColor					 = $AA97;
_PmBackColor					 = $AA98;
_AnimateEntry					 = $AA99;
_AnimatePalette 				 = $AA9A;
_GetEntryColor					 = $AA9B;
_SetEntryColor					 = $AA9C;
_GetEntryUsage					 = $AA9D;
_SetEntryUsage					 = $AA9E;
_CTab2Palette					 = $AA9F;
_Palette2CTab					 = $AAA0;
_CopyPalette					 = $AAA1;
_PMgrOp 						 = $A085;
_HUnmountVol					 = $A20E;

{  Windows   }

_BeginUpDate					 = $A922;
_BringToFront					 = $A920;
_CalcVBehind					 = $A90A;
_CalcVis						 = $A909;
_CheckUpDate					 = $A911;
_ClipAbove						 = $A90B;
_CloseWindow					 = $A92D;
_DisposWindow					 = $A914;
_DragGrayRgn					 = $A905;
_DragWindow 					 = $A925;
_DrawGrowIcon					 = $A904;
_DrawNew						 = $A90F;
_EndUpDate						 = $A923;
_FindWindow 					 = $A92C;
_FrontWindow					 = $A924;
_GetAuxWin						 = $AA42;
_GetCWMgrPort					 = $AA48;
_GetNewCWindow					 = $AA46;
_GetNewWindow					 = $A9BD;
_GetWindowPic					 = $A92F;
_GetWMgrPort					 = $A910;
_GetWRefCon 					 = $A917;
_GetWTitle						 = $A919;
_GetWVariant					 = $A80A;
_GrowWindow 					 = $A92B;
_HideWindow 					 = $A916;
_HiliteWindow					 = $A91C;
_InitWindows					 = $A912;
_InvalRect						 = $A928;
_InvalRgn						 = $A927;
_MoveWindow 					 = $A91B;
_NewCWindow 					 = $AA45;
_NewWindow						 = $A913;
_PaintBehind					 = $A90D;
_PaintOne						 = $A90C;
_PinRect						 = $A94E;
_SaveOld						 = $A90E;
_SelectWindow					 = $A91F;
_SendBehind 					 = $A921;
_SetDeskCPat					 = $AA47;
_SetWinColor					 = $AA41;
_SetWindowPic					 = $A92E;
_SetWRefCon 					 = $A918;
_SetWTitle						 = $A91A;
_ShowHide						 = $A908;
_ShowWindow 					 = $A915;
_SizeWindow 					 = $A91D;
_TrackBox						 = $A83B;
_TrackGoAway					 = $A91E;
_ValidRect						 = $A92A;
_ValidRgn						 = $A929;
_ZoomWindow 					 = $A83A;
_PutIcon						 = $A9CA;

{  Notification }

_NMInstall						 = $A05E;
_NMRemove						 = $A05F;

{  SCSI   }

_SCSIDispatch					 = $A815;

{  Script   }

_ScriptUtil 					 = $A8B5;

{  Slots 	}

_SlotManager					 = $A06E;

{  Sound 	}

_SndDoCommand					 = $A803;
_SndDoImmediate 				 = $A804;
_SndAddModifier 				 = $A802;
_SndNewChannel					 = $A807;
_SndDisposeChannel				 = $A801;
_SndControl 					 = $A806;
_SndPlay						 = $A805;

_HFSPinaforeDispatch			 = $AA52;
{$ENDC} { UsingTraps }

{$IFC NOT UsingIncludes}
	END.
{$ENDC}

