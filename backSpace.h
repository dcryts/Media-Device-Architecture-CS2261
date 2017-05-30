
//{{BLOCK(backSpace)

//======================================================================
//
//	backSpace, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 30 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 1920 + 2048 = 4480
//
//	Time-stamp: 2014-04-20, 20:37:40
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BACKSPACE_H
#define GRIT_BACKSPACE_H

#define backSpaceTilesLen 1920
extern const unsigned short backSpaceTiles[960];

#define backSpaceMapLen 2048
extern const unsigned short backSpaceMap[1024];

#define backSpacePalLen 512
extern const unsigned short backSpacePal[256];

#endif // GRIT_BACKSPACE_H

//}}BLOCK(backSpace)
