
//{{BLOCK(frontSpace)

//======================================================================
//
//	frontSpace, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 30 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 1920 + 2048 = 4480
//
//	Time-stamp: 2014-04-20, 20:36:56
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FRONTSPACE_H
#define GRIT_FRONTSPACE_H

#define frontSpaceTilesLen 1920
extern const unsigned short frontSpaceTiles[960];

#define frontSpaceMapLen 2048
extern const unsigned short frontSpaceMap[1024];

#define frontSpacePalLen 512
extern const unsigned short frontSpacePal[256];

#endif // GRIT_FRONTSPACE_H

//}}BLOCK(frontSpace)
