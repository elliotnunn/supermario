/*EASE$$$ READ ONLY COPY of file WindowMgr.r
** 1.0	BBM 2/12/88 Adding file for the first time into EASE…
** END EASE MODIFICATION HISTORY */
/*

	Toolbox ROMResources

	These are miscellaneous resources that are used by the high-level toolbox

		Modification History:
		---------------------
		<C443/19Nov86>	DAF			New today.  Includes wctb=0 and cctb=0.
		<C458/23Nov86>  DAF			Added PPAT=16.
		<C610/09Jan87>  EHB			changed 'PPAT' to 'ppat'
									fixed offsets in 'ppat'
		<C616/12Jan87> 	DAF			unified this version of ppat=16 with the system build
		<C725/31Jan87>  DAF			removed ppat=16
*/

		
data 'wctb' (0) {								/* default window colors */
		$"00000000"								/* seed field (unused) 	*/
		$"0000"									/* transIndex (unused) 	*/
		$"0004"									/* CTSize = 4 			*/
		$"0000FFFFFFFFFFFF"						/* wContentColor= white	*/		
		$"0001000000000000"						/* wFrameColor= black	*/		
		$"0002000000000000"						/* wTextColor= black	*/		
		$"0003000000000000"						/* wHiliteColor= black	*/		
		$"0004FFFFFFFFFFFF"						/* wTitleBarColor= white */		
};

data 'cctb' (0) {								/* default control colors */
		$"00000000"								/* seed field (unused) 	*/
		$"0000"									/* transIndex (unused) 	*/
		$"0003"									/* CTSize = 3 			*/
		$"0000000000000000"						/* cFrameColor= black	*/		
		$"0001FFFFFFFFFFFF"						/* cBodyColor= white 	*/		
		$"0002000000000000"						/* cTextColor= black	*/		
		$"0003FFFFFFFFFFFF"						/* cElevatorColor= white */		
};


