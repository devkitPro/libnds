/*---------------------------------------------------------------------------------

	Video API vaguely similar to OpenGL

  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/

#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>

// this is the actual data of the globals for videoGL
//   Please use the glGlob pointer to access this data since that makes it easier to move stuff in/out of the header.
gl_hidden_globals glGlobalData;



// This returns the pointer to the globals for videoGL
gl_hidden_globals* glGetGlobals() {
	return &glGlobalData;
}

//---------------------------------------------------------------------------------
void glRotatef32i(int angle, int32 x, int32 y, int32 z) {
//---------------------------------------------------------------------------------
	int32 axis[3];
	int32 sine = sinLerp(angle);//SIN[angle &  LUT_MASK];
	int32 cosine = cosLerp(angle);//COS[angle & LUT_MASK];
	int32 one_minus_cosine = inttof32(1) - cosine;

	axis[0]=x;
	axis[1]=y;
	axis[2]=z;

	normalizef32(axis);   // should require passed in normalized?

	MATRIX_MULT3x3 = cosine + mulf32(one_minus_cosine, mulf32(axis[0], axis[0]));
	MATRIX_MULT3x3 = mulf32(one_minus_cosine, mulf32(axis[0], axis[1])) + mulf32(axis[2], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) - mulf32(axis[1], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]),  axis[1]) - mulf32(axis[2], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[1]), axis[1]);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) + mulf32(axis[0], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) + mulf32(axis[1], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) - mulf32(axis[0], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[2]), axis[2]);
}




//---------------------------------------------------------------------------------
void glMaterialf(GL_MATERIALS_ENUM mode, rgb color) {
//---------------------------------------------------------------------------------
	static uint32 diffuse_ambient = 0;
	static uint32 specular_emission = 0;

	switch(mode) {
		case GL_AMBIENT:
			diffuse_ambient = (color << 16) | (diffuse_ambient & 0xFFFF);
			break;
		case GL_DIFFUSE:
			diffuse_ambient = color | (diffuse_ambient & 0xFFFF0000);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			diffuse_ambient= color + (color << 16);
			break;
		case GL_SPECULAR:
			specular_emission = color | (specular_emission & 0xFFFF0000);
			break;
		case GL_SHININESS:
			break;
		case GL_EMISSION:
			specular_emission = (color << 16) | (specular_emission & 0xFFFF);
			break;
	}

	GFX_DIFFUSE_AMBIENT = diffuse_ambient;
	GFX_SPECULAR_EMISSION = specular_emission;
}

//---------------------------------------------------------------------------------
void glTexCoord2f32(int32 u, int32 v) { 
//---------------------------------------------------------------------------------
	int x, y;
	gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, glGlob->activeTexture );
	if( tex ) {
		x = (tex->texFormat >> 20) & 7; 
		y = (tex->texFormat >> 23) & 7; 
		glTexCoord2t16(f32tot16 (mulf32(u,inttof32(8<<x))), f32tot16 (mulf32(v,inttof32(8<<y)))); 
	}
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// internal VRAM allocation/deallocation functions
//  ( calling these functions outside of videoGL may interfere with normal operations )

void
vramBlock_init( s_vramBlock *mb ) {
	// Construct a new block that will be set as the first block, as well as the first empty block
	struct s_SingleBlock *newBlock = (struct s_SingleBlock*)malloc( sizeof( struct s_SingleBlock ));
	memset( (void*)newBlock, 0, sizeof( s_SingleBlock ));
	newBlock->AddrSet = mb->startAddr;
	newBlock->blockSize = (uint32)mb->endAddr - (uint32)mb->startAddr;
	
	mb->firstBlock = mb->firstEmpty = newBlock;
	
	// Default settings and initializations for up to 16 blocks ( will increase	as limit is reached )
	mb->blockCount = 1;
	mb->deallocCount = 0;

	mb->lastExamined = NULL;
	mb->lastExaminedAddr = NULL;
	mb->lastExaminedSize = 0;

	DynamicArrayInit( &mb->blockPtrs, 16 );
	DynamicArrayInit( &mb->deallocBlocks, 16 );
	int i;
	for( i = 0; i < 16; i++ ) {
		DynamicArraySet( &mb->blockPtrs, i, (void*)0 );
		DynamicArraySet( &mb->deallocBlocks, i, (void*)0 );
	}	
}


s_vramBlock* vramBlock_Construct( uint8 *start, uint8 *end ) {
	// Block Container is constructed, with a starting and ending address. Then initialization of the first block is made
	struct s_vramBlock* mb = ( struct s_vramBlock*)malloc( sizeof( s_vramBlock ));
	if( start > end ) {
		mb->startAddr = end; mb->endAddr = start;
	} else {
		mb->startAddr = start; mb->endAddr = end;
	}
	vramBlock_init( mb );
	return mb;
}


void
vramBlock_terminate( s_vramBlock *mb ) {
	// Starts at the container's first block, and goes through each sequential block, deleting them 
	struct s_SingleBlock *curBlock = mb->firstBlock;

	while( curBlock != NULL ) {
		struct s_SingleBlock *nextBlock = curBlock->node[ 1 ];
		free( curBlock );
		curBlock = nextBlock;
	}		

	DynamicArrayDelete( &mb->deallocBlocks );
	DynamicArrayDelete( &mb->blockPtrs );
}


void vramBlock_Deconstruct( s_vramBlock *mb ) {
	// Container must exist for deconstructing
	if( mb ) {
		vramBlock_terminate( mb );
		free( mb );
	}
}

uint8*
vramBlock__allocateBlock( s_vramBlock *mb, struct s_SingleBlock *block, uint8 *addr, uint32 size ) {
	// Initial tests to ensure allocation is valid
	if( !size || !addr || !block || block->indexOut || addr < block->AddrSet || ( addr + size ) > ( block->AddrSet + block->blockSize )) return 0;

	// Get pointers to the various blocks, as those may change from allocation
	struct s_SingleBlock **first = &mb->firstBlock, **alloc = &mb->firstAlloc, **empty = &mb->firstEmpty;
	// The nodes in the test block array will change as examinations of pre/post blocks are done
	struct s_SingleBlock *testBlock[ 4 ] = { block->node[ 0 ], block->node[ 1 ], block->node[ 2 ], block->node[ 3 ] };
	
	// Boolean comparisons ( for determining if an empty block set for allocation should be split once, twice, or not at all )
	uint32 valComp[ 2 ] = { addr != block->AddrSet, addr + size < block->AddrSet + block->blockSize };
	
	int i = 0;
	for( ; i < 2; i++ ) {
		// Generate a new block if condition requires it, based on earlier comparison
		if( valComp[ i ] ) {
			// If comparison check is true, then empty block is split in two empty blocks. Addresses set, block sizes corrected, and
			//			nodes linked between the two. This can be done up to two times, resulting in 3 empty blocks sequentially.
			//			The middle in this case will be the true allocated block. If split once in total, either block will be
			//			the true block. Also done is examination of the first block and first empty block, which will be set as well
			struct s_SingleBlock *newBlock = ( struct s_SingleBlock* )malloc( sizeof( struct s_SingleBlock ));
			newBlock->indexOut = 0;
			newBlock->AddrSet = block->AddrSet + ( i * size );
					
			if( i ) {
				newBlock->blockSize = block->blockSize - size;
				block->blockSize = size;
			} else {
				newBlock->blockSize = (uint32)addr - (uint32)block->AddrSet;
				block->AddrSet = addr;
				block->blockSize -= newBlock->blockSize;
				if( block == *first )
					*first = newBlock;
			}
			
			// To get an idea of what the nodes represent, the first two in the array refer to the immediate pre/post blocks, which
			//		can be either empty or allocated blocks. The last two in the array refer to the prior/next blocks of the same type
			//		(empty or allocated), which can skip over blocks of a differing type. This allows for quick examination of similar
			//		blocks while keeping the organization of all the blocks in an ordered fashion. Think of it like a doubly-doubly
			//		linked list
			newBlock->node[ 1 - i ] = block;
			newBlock->node[ i ] = testBlock[ i ];
			newBlock->node[ i + 2 ] = testBlock[ i + 2 ];
				
			block->node[ i ] = newBlock;
			if( testBlock[ i ] )
				testBlock[ i ]->node[ 1 - i ] = newBlock;
			if( testBlock[ i + 2 ] )
				testBlock[ i + 2 ]->node[ 3 - i ] = newBlock;
					
			testBlock[ i + 2 ] = newBlock;
				
			if( block == *empty )
				*empty = newBlock;
		}
	}
	
	// Finish off node linking (in this case, NULL is possible, which refers to the end of the block sequences)
	if( testBlock[ 2 ] )
		testBlock[ 2 ]->node[ 3 ] = testBlock[ 3 ];
	if( testBlock[ 3 ] )
		testBlock[ 3 ]->node[ 2 ] = testBlock[ 2 ];
	
	// The condition of examining the first empty block is needed, in case the comparison check early is false for both
	if( block == *empty )
		*empty = block->node[ 3 ];
		
	block->node[ 2 ] = testBlock[ 0 ];
	block->node[ 3 ] = testBlock[ 1 ];
	if( testBlock[ 0 ] )
		testBlock[ 0 ]->node[ 3 ] = block;
	else
		*alloc = block;
	if( testBlock[ 1 ] )
		testBlock[ 1 ]->node[ 2 ] = block;
		
	return (uint8*)block;
}

uint32
vramBlock__deallocateBlock( s_vramBlock *mb, struct s_SingleBlock *block ) {
	// Test to see if this is an allocated block
	if( !block->indexOut ) return 0;
	struct s_SingleBlock **first = &mb->firstBlock, **alloc = &mb->firstAlloc, **empty = &mb->firstEmpty;
	
	// Unlike empty blocks, allocated blocks can be next to each other to help retain the actual spaces being allocated.
	//		This is why when contructing the Test Block for the deallocation process, it fills it with the prior/next links for
	//		both sets instead of including the immediate pre/post blocks for figuring out the prior/next closest empty block
	struct s_SingleBlock *testBlock[ 4 ] = { block->node[ 2 ], block->node[ 3 ], block->node[ 2 ], block->node[ 3 ] };
		
	int i = 0;
	for( ; i < 2; i++ ) {
		// If the immediate prior/next test link is not the block's immediate prior/next link (meaning an empty block
		//		separates them), then set the prior/next link to that empty block
		if( testBlock[ i ] != block->node[ i ] )
			testBlock[ i + 2 ] = block->node[ i ];
		else {
			// If not, then scan through the prior/next links until either an empty block is found, or no blocks (NULL) are found
			while( testBlock[ i + 2 ] && testBlock[ i + 2 ]->indexOut )
				testBlock[ i + 2 ] = testBlock[ i + 2 ]->node[ i ];
		}
	}
	
	// Begin initial "rewiring" stage for when the block transitions from allocated to empty
	if( testBlock[ 0 ] )	testBlock[ 0 ]->node[ 3 ] = testBlock[ 1 ];
	if( testBlock[ 1 ] )	testBlock[ 1 ]->node[ 2 ] = testBlock[ 0 ];
	if( testBlock[ 2 ] )	testBlock[ 2 ]->node[ 3 ] = block;
	if( testBlock[ 3 ] )	testBlock[ 3 ]->node[ 2 ] = block;
	
	block->node[ 2 ] = testBlock[ 2 ];
	block->node[ 3 ] = testBlock[ 3 ];
	block->indexOut = 0;
	
	// If this block was the first allocated block of the group, then pass allocation lead to next one, even if that is NULL
	if( block == *alloc )
		*alloc = testBlock[ 1 ];
		
	for( i = 0; i < 2; i++ ) {
		if( testBlock[ i + 2 ] ) {
			// If true, then we must do more rewiring, as well as merging blocks together. This also includes reassigning
			//		the first block and first empty block if necessary
			if( testBlock[ i + 2 ] == block->node[ i ] ) {
				block->node[ i ] = testBlock[ i + 2 ]->node[ i ];
				if( block->node[ i ] )		block->node[ i ]->node[ 1 - i ] = block;
				block->node[ i + 2 ] = testBlock[ i + 2 ]->node[ i + 2 ];
				if( block->node[ i + 2 ] )		block->node[ i + 2 ]->node[ 3 - i ] = block;
				block->blockSize += testBlock[ i + 2 ]->blockSize;
				if( !i ) {
					block->AddrSet = testBlock[ 2 ]->AddrSet;
					if( testBlock[ 2 ] == *first )
						*first = block;
				}
				if( testBlock[ i + 2 ] == *empty )
						*empty = block;
				free( testBlock[ i + 2 ] );
			// Even if the above did not happen, there is still a chance the new deallocated block may now be the first
			//	empty block, so assign it if that is the case
			} else if( i && testBlock[ i + 2 ] == *empty )
				*empty = block;
		}
	}
	
	return 1;
}

uint8*
vramBlock_examineSpecial( s_vramBlock *mb, uint8 *addr, uint32 size, uint8 align ) {
	// Simple validity tests
	if( !addr || !mb->firstEmpty || !size || align >= 8 )
		return NULL;
	// Start with the first empty block
	struct s_SingleBlock *block = mb->firstEmpty;
	
	// Set these value to 0/NULL (should only be filled in with valid data in case of error), and copy the address to start checking
	mb->lastExamined = NULL;
	mb->lastExaminedAddr = NULL;
	mb->lastExaminedSize = 0;
	uint8 *checkAddr = addr;
	
	// If the address is within a valid block, examine if it will initially fit in it
	while( block && checkAddr >= block->AddrSet + block->blockSize )
		block = block->node[ 3 ];
	if( !block ) return NULL;

	// Move the address up if before the first valid block
	if( checkAddr < block->AddrSet )
		checkAddr = block->AddrSet;
		
	uint8* bankLock[ 5 ] = { 0x0 };
	uint32 bankSize[ 5 ] = { 0x0 };
	uint32 curBank = 0;
	
	// Values that hold which banks to examine
	uint32 isNotMainBank = ( checkAddr >= (uint8*)VRAM_E ? 1 : 0 );
	uint32 vramCtrl = ( isNotMainBank ? VRAM_EFG_CR : VRAM_CR );
	uint32 i = 0, iEnd = ( isNotMainBank ? 3 : 4 );
	
	// Fill in the array with only those banks that are not set for Textures or Texture Palettes
	for( ; i < iEnd; i++ ) {
		if(( vramCtrl & 0x83 ) != 0x83 ) {		// VRAM_ENABLE | ( VRAM_x_TEXTURE | VRAM_x_TEX_PALETTE )
			if( isNotMainBank ) {
				bankLock[ curBank ] = ( i == 0 ? (uint8*)VRAM_E : (uint8*)VRAM_F + (( i - 1 ) * 0x4000 ));
				bankSize[ curBank ] = ( i == 0 ? 0x10000 : 0x4000 );
			} else {
				bankLock[ curBank ] = (uint8*)VRAM_A + ( i * 0x20000 );
				bankSize[ curBank ] = 0x20000;
			}
			curBank++;
		}
		vramCtrl >>= 8;
	}
	curBank = 0;

	// Retrieve the available area from this block using the address given
	uint32 curBlockSize = block->blockSize - ((uint32)checkAddr - (uint32)block->AddrSet );
	do {
		// Do address adjustments based on locked banks
		if( bankLock[ curBank ] ) {
			// Skip to corresponding bank that address is in
			while( bankLock[ curBank ] && checkAddr >= ( bankLock[ curBank ] + bankSize[ curBank ] ))
				curBank++;
			do {
				// Examine is address is within locked bank, and push it to next bank if needed
				if( bankLock[ curBank ] && checkAddr >= bankLock[ curBank ] && checkAddr < bankLock[ curBank ] + bankSize[ curBank ] )
					checkAddr = bankLock[ curBank ] + bankSize[ curBank ];
				else
					break;
			} while( bankLock[ ++curBank ] != NULL );	
			// Continue block and address adjustments
			while( block && checkAddr >= block->AddrSet + block->blockSize )
				block = block->node[ 3 ];
			if( !block )	return NULL;
			if( checkAddr < block->AddrSet )
				checkAddr = block->AddrSet;
			// Adjust the blocks available size based on address location within said block
			if( bankLock[ curBank ] && bankLock[ curBank ] < block->AddrSet + block->blockSize )
				curBlockSize = (uint32)bankLock[ curBank ] - (uint32)checkAddr;
			else
				curBlockSize = block->blockSize - ((uint32)checkAddr - (uint32)block->AddrSet );
		}
		
		// Obtained an aligned address, and adjust the available area that can be used
		uint8* aligned_checkAddr = (uint8*)(((uint32)checkAddr + ((1<<align)-1)) & (~((1<<align)-1)));
		uint32 excess = ((uint32)aligned_checkAddr - (uint32)checkAddr);
		curBlockSize -= excess;
		if( curBlockSize >= size ) {
			mb->lastExamined = block;
			mb->lastExaminedAddr = aligned_checkAddr;
			mb->lastExaminedSize = size;
			return aligned_checkAddr;
		} else {
			if( bankLock[ curBank ] && bankLock[ curBank ] < block->AddrSet + block->blockSize ) {
				checkAddr = bankLock[ curBank ] + bankSize[ curBank ];
				curBlockSize = 0;
			} else {
				block = block->node[ 3 ];
				if( !block )	return NULL;
				checkAddr = block->AddrSet;
				curBlockSize = block->blockSize;
			}
		}
	} while( block != NULL );
	return NULL;
}

uint32
vramBlock_allocateSpecial( s_vramBlock *mb, uint8 *addr, uint32 size ) {
	// Simple validity tests. Special allocations require "examination" data
	if( !addr || !size || !mb->lastExamined || !mb->lastExaminedAddr ) return 0;
	if( mb->lastExaminedAddr != addr || mb->lastExaminedSize != size ) return 0;
	
	// Can only get here if prior tests passed, meaning a spot is available, and can be allocated
	struct s_SingleBlock *newBlock = (struct s_SingleBlock*)vramBlock__allocateBlock( mb,  mb->lastExamined, addr, size );
	if( newBlock ) { // with current implementation, it should never be false if it gets to here
		uint32 curBlock;
		// Use a prior index if one exists. Else, obtain a new index
		if( mb->deallocCount )
			curBlock = (uint32)DynamicArrayGet( &mb->deallocBlocks, mb->deallocCount-- );
		else
			curBlock = mb->blockCount++;
		DynamicArraySet( &mb->blockPtrs, curBlock, (void*)newBlock );
		// Clear out examination data
		mb->lastExamined = NULL;
		mb->lastExaminedAddr = NULL;
		mb->lastExaminedSize = 0;
		newBlock->indexOut = curBlock;	
		return curBlock;
	}
	return 0;
}

uint32
vramBlock_allocateBlock( s_vramBlock *mb, uint32 size, uint8 align ) {
	// Simple valid tests, such as if there are no more empty blocks as indicated by "firstEmpty"
	if( mb->firstEmpty == NULL || !size || align >= 8 )
		return 0;

	// Grab the first empty block, and begin examination for a valid spot from there
	struct s_SingleBlock *block = mb->firstEmpty;
	uint8* checkAddr = vramBlock_examineSpecial( mb, block->AddrSet, size, align );
	if( !checkAddr ) return 0;
	
	// If execution gets here, then a spot was found, so allocate it
	return vramBlock_allocateSpecial( mb, checkAddr, size );
}

uint32
vramBlock_deallocateBlock( s_vramBlock *mb, uint32 index ) {
	// Retrieve the block from the index array, and see if it exists. If it does, and is deallocated (which it should),
	//		remove from index list
	struct s_SingleBlock *block = (struct s_SingleBlock*)DynamicArrayGet( &mb->blockPtrs, index );
	if( block && vramBlock__deallocateBlock( mb, block )) {
		DynamicArraySet( &mb->blockPtrs, index, (void*)0x0 );
		DynamicArraySet( &mb->deallocBlocks, ++mb->deallocCount, (void*)index );
		return 1;
	}
	return 0;
}

void
vramBlock_deallocateAll( s_vramBlock *mb ) {
	// Reset the entire container
	vramBlock_terminate( mb );
	vramBlock_init( mb );
}


uint8*
vramBlock_getAddr( s_vramBlock *mb, uint32 index ) {
	struct s_SingleBlock *getBlock;
	if(( getBlock = (struct s_SingleBlock*)DynamicArrayGet( &mb->blockPtrs, index )))
		return getBlock->AddrSet;
	return NULL;
}


uint32
vramBlock_getSize( s_vramBlock *mb, uint32 index ) {
	struct s_SingleBlock *getBlock;
	if(( getBlock = (struct s_SingleBlock*)DynamicArrayGet( &mb->blockPtrs, index )))
		return getBlock->blockSize;
	return 0;
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
void glInit_C(void) {
//---------------------------------------------------------------------------------
	int i;

	powerOn(POWER_3D_CORE | POWER_MATRIX);	// enable 3D core & geometry engine

	glGlob = glGetGlobals();

	if( glGlob->isActive )
		return;
	
	// Allocate the designated layout for each memory block
	glGlob->vramBlocks[ 0 ] = vramBlock_Construct( (uint8*)VRAM_A, (uint8*)VRAM_E );
	glGlob->vramBlocks[ 1 ] = vramBlock_Construct( (uint8*)VRAM_E, (uint8*)VRAM_H );

	// init texture globals
	
	glGlob->clearColor = 0;
	
	glGlob->activeTexture = 0;
	glGlob->activePalette = 0;
	glGlob->texCount = 1;
	glGlob->palCount = 1; 
	glGlob->deallocTexSize = 0;
	glGlob->deallocPalSize = 0;

	// Clean out all this crap
	DynamicArrayInit( &glGlob->texturePtrs, 16 );
	DynamicArrayInit( &glGlob->palettePtrs, 16 );
	DynamicArrayInit( &glGlob->deallocTex, 16 );
	DynamicArrayInit( &glGlob->deallocPal, 16 );
	
	for(i = 0; i < 16; i++) {
		DynamicArraySet( &glGlob->texturePtrs, i, (void*)0 );
		DynamicArraySet( &glGlob->palettePtrs, i, (void*)0 );
		DynamicArraySet( &glGlob->deallocTex, i, (void*)0 );
		DynamicArraySet( &glGlob->deallocPal, i, (void*)0 );
	}
	
	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy

	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows from list memory
	glResetMatrixStack();

	// prime the vertex/polygon buffers
	glFlush(0);

	// reset the control bits
	GFX_CONTROL = 0;

	// reset the rear-plane(a.k.a. clear color) to black, ID=0, and opaque
	glClearColor(0,0,0,31);
	glClearPolyID(0);

	// reset the depth to it's max
	glClearDepth(GL_MAX_DEPTH);

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glGlob->isActive = 1;
}

//---------------------------------------------------------------------------------
void glResetTextures(void) {
//---------------------------------------------------------------------------------
	int i;

	glGlob->activeTexture = 0;
	glGlob->activePalette = 0;
	glGlob->texCount = 1;
	glGlob->palCount = 1;
	glGlob->deallocTexSize = 0;
	glGlob->deallocPalSize = 0;

	// Any textures in use will be clean of all their data
	for(i = 0; i < (int)glGlob->texturePtrs.cur_size; i++) {
		gl_texture_data* texture = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, i );
		if( texture ) {
			free( texture );
			DynamicArraySet(&glGlob->texturePtrs, i, (void*)0 );
		}
	}

	// Any palettes in use will be cleaned of all their data
	for( i = 0; i < (int)glGlob->palettePtrs.cur_size; i++ ) {
		gl_palette_data* palette = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, i );
		if( palette ) {
			free( palette );
			DynamicArraySet( &glGlob->palettePtrs, i, (void*)0 );
		}
	}

	// Clean out both blocks
	for( i = 0; i < 2; i++ )
		vramBlock_deallocateAll( glGlob->vramBlocks[ i ] );
}


void removePaletteFromTexture( gl_texture_data *tex ) {
	if( tex ) {
		gl_palette_data *palette = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, tex->palIndex );
		if( palette->connectCount ) {
			if( !(--palette->connectCount)) {
				DynamicArraySet( &glGlob->deallocPal, ++glGlob->deallocPalSize, (void*)tex->palIndex );
				vramBlock_deallocateBlock( glGlob->vramBlocks[ 1 ], palette->palIndex );
				free( palette );
				if( glGlob->activePalette == tex->palIndex )
					GFX_PAL_FORMAT = glGlob->activePalette = 0;
				DynamicArraySet( &glGlob->palettePtrs, tex->palIndex, (void*)0 );	
			}
			tex->palIndex = 0;
		}
	}
}

//---------------------------------------------------------------------------------
//	glGenTextures creates integer names for your table
//	takes n as the number of textures to generate and 
//	a pointer to the names array that it needs to fill.
//  Returns 1 if succesful and 0 if out of texture names
//---------------------------------------------------------------------------------

int glGenTextures(int n, int *names) {
//---------------------------------------------------------------------------------
	int index = 0;
	
	// Don't do anything if can't add all generated textures
	if(( glGlob->texCount - glGlob->deallocTexSize ) + n >= MAX_TEXTURES )
		return 0;
	
	// Generate texture names for each element
	for(index = 0; index < n; index++) {
		gl_texture_data *texture = (gl_texture_data*)malloc( sizeof( gl_texture_data ));
		memset( (void*)texture, 0, sizeof( gl_texture_data ));
		if( glGlob->deallocTexSize  ) // Use previously deleted texture names
			names[ index ] = (uint32)DynamicArrayGet( &glGlob->deallocTex, glGlob->deallocTexSize-- );
		else
			names[ index ] = glGlob->texCount++;
		DynamicArraySet( &glGlob->texturePtrs, names[ index ], (void*)texture  );
	}
	
	return 1;
}


//---------------------------------------------------------------------------------
//	glDeleteTextures deletes integer names from your table
//	takes n as the number of textures to delete and 
//	a pointer to the names array that it needs to remove.
//  Returns 1 if succesful and 0 if out of texture names
//---------------------------------------------------------------------------------

int glDeleteTextures( int n, int *names ) {
//---------------------------------------------------------------------------------
	int index = 0;
	for(index = 0; index < n; index++) {
		if( names[ index ] != 0 ) {
			if( glGlob->deallocTexSize == MAX_TEXTURES || names[ index ] >= MAX_TEXTURES ) // This still needed?
				return 0;
			DynamicArraySet( &glGlob->deallocTex, ++glGlob->deallocTexSize, (void*)names[ index ] );
			gl_texture_data *texture = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, names[ index ] );
			if( texture ) {
				// Clear out the texture blocks
				if( texture->texIndex ) {
					if( texture->texIndexExt )	// Delete extra texture for GL_COMPRESSED, if exists
						vramBlock_deallocateBlock( glGlob->vramBlocks[ 0 ], texture->texIndexExt );
					if( texture->texIndex )
						vramBlock_deallocateBlock( glGlob->vramBlocks[ 0 ], texture->texIndex );
				}
				// Clear out the palette (if this texture name is the last using it)
				if( texture->palIndex )
					removePaletteFromTexture( texture );
				free( texture );
			}
			DynamicArraySet( &glGlob->texturePtrs, names[ index ], (void*)0 );
			// Zero out register if the active texture was being deleted
			if( glGlob->activeTexture == names[ index ] ) {
				GFX_TEX_FORMAT = 0;
				glGlob->activeTexture = 0;
			}
			names[ index ] = 0;
		}
	}
	return 1;
}

//---------------------------------------------------------------------------------
uint16* vramGetBank(uint16 *addr) {
//---------------------------------------------------------------------------------
	if(addr >= VRAM_A && addr < VRAM_B)
		return VRAM_A;
	else if(addr >= VRAM_B && addr < VRAM_C)
		return VRAM_B;
	else if(addr >= VRAM_C && addr < VRAM_D)
		return VRAM_C;
	else if(addr >= VRAM_D && addr < VRAM_E)
		return VRAM_D;
	else if(addr >= VRAM_E && addr < VRAM_F)
		return VRAM_E;
	else if(addr >= VRAM_F && addr < VRAM_G)
		return VRAM_F;
	else if(addr >= VRAM_G && addr < VRAM_H)
		return VRAM_G;
	else if(addr >= VRAM_H && addr < VRAM_I)
		return VRAM_H;
	else return VRAM_I;
}

//---------------------------------------------------------------------------------
// glBindTexure sets the current named
//	texture to the active texture.  Target 
//	is ignored as all DS textures are 2D
//---------------------------------------------------------------------------------
void glBindTexture(int target, int name) {
//---------------------------------------------------------------------------------
	gl_texture_data *tex = NULL;
	// no reason to process if name is the active texture
	if( glGlob->activeTexture == name ) return;

	// name exist?
	if(( tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, name ))) {
		GFX_TEX_FORMAT = tex->texFormat;
		glGlob->activeTexture = name;
		// Set palette if exists
		if( tex->palIndex ) {
			gl_palette_data *pal = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, tex->palIndex );
			GFX_PAL_FORMAT = pal->addr;
			glGlob->activePalette = tex->palIndex;
		} else
			GFX_PAL_FORMAT = glGlob->activePalette = 0;
	} else {
		GFX_TEX_FORMAT = GFX_PAL_FORMAT = glGlob->activePalette = glGlob->activeTexture = 0;
	}
}


//---------------------------------------------------------------------------------
// glColorTableEXT loads a 15-bit color
//  format palette into palette memory,
//  and sets it to the currently bound texture
//---------------------------------------------------------------------------------
void glColorTableEXT( int target, int empty1, uint16 width, int empty2, int empty3, const uint16* table ) {
//---------------------------------------------------------------------------------
	if( glGlob->activeTexture ) {
		gl_texture_data *texture = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, glGlob->activeTexture );
		gl_palette_data *palette;
		if( texture->palIndex )	// Remove prior palette if exists
			removePaletteFromTexture( texture );

		// Exit if no color table or color count is 0 (helpful in emptying the palette for the active texture)
		if( !width || table == NULL )
			return;

		// Allocate new palette block based on the texture's format
		uint32 colFormat = (( texture->texFormat >> 26 ) & 0x7 );
		
		uint32 colFormatVal = (( colFormat == GL_RGB4 || ( colFormat == GL_NOTEXTURE && width <= 4 )) ? 3 : 4 );
		uint8* checkAddr = vramBlock_examineSpecial( glGlob->vramBlocks[ 1 ], (uint8*)VRAM_E, width << 1, colFormatVal );
		
		if( checkAddr ) {
			// Calculate the address, logical and actual, of where the palette will go
			uint16* baseBank = vramGetBank( (uint16*)checkAddr );
			uint32 addr = ( (uint32)checkAddr - (uint32)baseBank );
			uint8 offset = 0;
			
			if( baseBank == VRAM_F )
				offset = ( VRAM_F_CR >> 3 ) & 3;
			else if( baseBank == VRAM_G )
				offset = ( VRAM_G_CR >> 3 ) & 3;
			addr += (( offset & 0x1 ) * 0x4000 ) + (( offset & 0x2 ) * 0x8000 ); 
			
			addr >>= colFormatVal;
			if( colFormatVal == 3 && addr >= 0x2000 ) {
				// palette location not good because 4 color mode cannot extend past 64K texture palette space
				GFX_PAL_FORMAT = glGlob->activePalette = 0;
				return;
			}
			
			palette = (gl_palette_data*)malloc( sizeof( gl_palette_data ));
			palette->palIndex = vramBlock_allocateSpecial( glGlob->vramBlocks[ 1 ], checkAddr, width << 1 ); 
			palette->vramAddr = checkAddr;
			palette->addr = addr;
			
			palette->connectCount = 1;
			palette->palSize = width << 1;
			
			// copy straight to VRAM, and assign a palette name
			uint32 tempVRAM = vramSetBanks_EFG( VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD );
			swiCopy( table, palette->vramAddr, width | COPY_MODE_HWORD );
			vramRestoreBanks_EFG( tempVRAM );

			if( glGlob->deallocPalSize )
				texture->palIndex = (uint32)DynamicArrayGet( &glGlob->deallocPal, glGlob->deallocPalSize-- );
			else
				texture->palIndex = glGlob->palCount++;
			DynamicArraySet( &glGlob->palettePtrs, texture->palIndex, (void*)palette );

			GFX_PAL_FORMAT = palette->addr;
			glGlob->activePalette = texture->palIndex;
		} else
			GFX_PAL_FORMAT = glGlob->activePalette = texture->palIndex;
	}
}

//---------------------------------------------------------------------------------
// glColorSubTableEXT loads a 15-bit color
//  format palette into a specific spot in
//  a currently bound texture's existing palette
//---------------------------------------------------------------------------------
void glColorSubTableEXT( int target, int start, int count, int empty1, int empty2, const uint16* data ) {
	if( count > 0 && glGlob->activePalette ) {
		gl_palette_data *palette = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, glGlob->activePalette );
		if( start >= 0 && ( start + count ) <= ( palette->palSize >> 1 ) ) {
			uint32 tempVRAM = vramSetBanks_EFG( VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD );
			swiCopy( data, palette->vramAddr + ( start << 1 ), count | COPY_MODE_HWORD );
			vramRestoreBanks_EFG( tempVRAM );
		}
	}
}

//---------------------------------------------------------------------------------
// glGetColorTableEXT retrieves a 15-bit color
//  format palette from the palette memory
//  of the currently bound texture
//---------------------------------------------------------------------------------
void glGetColorTableEXT( int target, int empty1, int empty2, uint16* table ) {
	if( glGlob->activePalette ) {
		gl_palette_data *palette = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, glGlob->activePalette );
		uint32 tempVRAM = vramSetBanks_EFG( VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD );
		swiCopy( palette->vramAddr, table, palette->palSize >> 1 | COPY_MODE_HWORD );
		vramRestoreBanks_EFG( tempVRAM );
	}
}
 
//---------------------------------------------------------------------------------
// nglAssignColorTable sets the active texture
//  with a palette set with another texture.
//  This is not an actual openGL function
//---------------------------------------------------------------------------------
void glAssignColorTable( int target, int name ) {
//---------------------------------------------------------------------------------
	// Allow assigning from a texture different from the active one
	if( glGlob->activeTexture && glGlob->activeTexture != name ) {
		gl_texture_data *texture = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs , glGlob->activeTexture );
		gl_texture_data *texCopy = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs , name );
		gl_palette_data *palette;
		if( texture->palIndex ) // Remove prior palette if exists
			removePaletteFromTexture( texture );

		if( texCopy && texCopy->palIndex ) {
			
			texture->palIndex = texCopy->palIndex;
			palette = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, texture->palIndex );
			palette->connectCount++;
			GFX_PAL_FORMAT = palette->addr;
			glGlob->activePalette = texture->palIndex;
		}
		else
			GFX_PAL_FORMAT = glGlob->activePalette = texture->palIndex = 0;
	}
}
                     
//---------------------------------------------------------------------------------
// glTexParameter although named the same 
//	as its gl counterpart it is not compatible
//	Effort may be made in the future to make it so.
//---------------------------------------------------------------------------------
void glTexParameter( int target, int param ) {
//---------------------------------------------------------------------------------
	if( glGlob->activeTexture ) {
		gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs , glGlob->activeTexture );
		GFX_TEX_FORMAT = tex->texFormat = ( tex->texFormat & 0x1FF0FFFF ) | param;
	}
	else
		GFX_TEX_FORMAT = 0;
}
//---------------------------------------------------------------------------------
//glGetTexturePointer gets a pointer to vram which contains the texture
//
//---------------------------------------------------------------------------------
void* glGetTexturePointer(	int name ) {
//---------------------------------------------------------------------------------
	gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, name );
	if( tex )
		return tex->vramAddr;
	else
		return NULL;
}

//---------------------------------------------------------------------------------
//glGetTexParameter retrieves the currently bound texture's format
//---------------------------------------------------------------------------------
u32 glGetTexParameter() {
//---------------------------------------------------------------------------------
	if( glGlob->activeTexture ) {
		gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, glGlob->activeTexture );
		return ( tex->texFormat );
	}
	return 0;
}

//---------------------------------------------------------------------------------
//glGetColorTableParameterEXT retrieves information
//  pertaining to the currently bound texture's palette
//---------------------------------------------------------------------------------
void glGetColorTableParameterEXT( int target, int pname, int * params ) {
//---------------------------------------------------------------------------------
	if( glGlob->activePalette ) {
		gl_palette_data *pal = (gl_palette_data*)DynamicArrayGet( &glGlob->palettePtrs, glGlob->activePalette );
		if( pname == GL_COLOR_TABLE_FORMAT_EXT )
			*params =  pal->addr;
		else if( pname == GL_COLOR_TABLE_WIDTH_EXT )
			*params = pal->palSize >> 1;
		else
			*params = -1;
	} else
		*params = -1;

}

//---------------------------------------------------------------------------------
// Similer to glTextImage2D from gl it takes a pointer to data
//  Empty fields and target are unused but provided for code compatibility.
//  type is simply the texture type (GL_RGB, GL_RGB8 ect...)
//---------------------------------------------------------------------------------
int glTexImage2D(int target, int empty1, GL_TEXTURE_TYPE_ENUM type, int sizeX, int sizeY, int empty2, int param, const void* texture) {
//---------------------------------------------------------------------------------
	uint32 size = 0;
	uint32 typeSizes[ 9 ] = { 0, 8, 2, 4, 8, 3, 8, 16, 16 };	// Represents the number of bits per pixels for each format
	
	if( !glGlob->activeTexture ) return 0;

	size = 1 << (sizeX + sizeY + 6);

	switch (type) {
		case GL_RGB:
		case GL_RGBA:
			size = size << 1;
			break;
		case GL_RGB4:
		case GL_COMPRESSED:
			size = size >> 2;
			break;
		case GL_RGB16:
			size = size >> 1;
			break;
		default:
			break;
	}
	if( !size ) return 0;
	
	if( type == GL_NOTEXTURE )
		size = 0;

	gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet( &glGlob->texturePtrs, glGlob->activeTexture );

	// Clear out the texture data if one already exists for the active texture
	if( tex ) {
		uint32 texType = (( tex->texFormat >> 26 ) & 0x07 );
		if(( tex->texSize != size ) || ( typeSizes[ texType ] != typeSizes[ type ] )) {
			if( tex->texIndexExt )
				vramBlock_deallocateBlock( glGlob->vramBlocks[ 0 ], tex->texIndexExt );
			if( tex->texIndex )
				vramBlock_deallocateBlock( glGlob->vramBlocks[ 0 ], tex->texIndex );
			tex->texIndex = tex->texIndexExt = 0;
			tex->vramAddr = NULL;
		}
	} 
	
	
	tex->texSize = size;
	
	// Allocate a new space for the texture in VRAM
	if( !tex->texIndex ) {
		if( type != GL_NOTEXTURE ) {
			if( type != GL_COMPRESSED ) {
				tex->texIndex = vramBlock_allocateBlock( glGlob->vramBlocks[ 0 ], tex->texSize, 3 );
			}
			else {
				uint8 *vramBAddr = (uint8*)VRAM_B;
				uint8 *vramACAddr = NULL;
				uint8 *vramBFound, *vramACFound;
				uint32 vramBAllocSize = size >> 1;
				if(( VRAM_B_CR & 0x83 )  != 0x83 )
					return 0;
			
				// Process of finding a valid spot for compressed textures is as follows...
				//		Examine first available spot in VRAM_B for the header data
				//		Extrapulate where the tile data would go in VRAM_A or VRAM_C if the spot in VRAM_B were used
				//		Check the extrapulated area to see if it is an empty spot
				//			If not, then adjust the header spot in VRAM_B by a ratio amount found by the tile spot
				while ( 1 ) {
					// Check designated opening, and return available spot
					vramBFound = vramBlock_examineSpecial( glGlob->vramBlocks[ 0 ], vramBAddr, vramBAllocSize, 2 );
					// Make sure that the space found in VRAM_B is completely in it, and not extending out of it
					if( vramGetBank( (uint16*)vramBFound ) != VRAM_B || vramGetBank( (uint16*)( vramBFound + vramBAllocSize ) - 1 ) != VRAM_B ) {
						return 0;
					}
					// Make sure it is completely on either half of VRAM_B
					if(((uint32)vramBFound - (uint32)VRAM_B < 0x10000 ) && ((uint32)vramBFound - (uint32)VRAM_B + vramBAllocSize > 0x10000 )) {
						vramBAddr = (uint8*)VRAM_B + 0x10000;
						continue;
					}
					// Retrieve the tile location in VRAM_A or VRAM_C
					uint32 offset = ((uint32)vramBFound - (uint32)VRAM_B );
					vramACAddr = (uint8*)( offset >= 0x10000 ? VRAM_B : VRAM_A ) + ( offset << 1 );
					vramACFound = vramBlock_examineSpecial( glGlob->vramBlocks[ 0 ], vramACAddr, size, 3 );
					if( vramACAddr != vramACFound ) {
						// Adjust the spot in VRAM_B by the difference found with VRAM_A/VRAM_C, divided by 2
						vramBAddr += (((uint32)vramACFound - (uint32)vramACAddr ) >> 1 );
						continue;
					} else {
						// Spot found, setting up spots
						tex->texIndex = vramBlock_allocateSpecial( glGlob->vramBlocks[ 0 ], vramACFound, size );
						tex->texIndexExt = vramBlock_allocateSpecial( glGlob->vramBlocks[ 0 ], vramBlock_examineSpecial( glGlob->vramBlocks[ 0 ], vramBFound, vramBAllocSize, 2 ), vramBAllocSize );
						break;
					}				
				}
			}
		}
		if( tex->texIndex ) {
			tex->vramAddr = vramBlock_getAddr( glGlob->vramBlocks[ 0 ], tex->texIndex );
			tex->texFormat = (sizeX << 20) | (sizeY << 23) | ((type == GL_RGB ? GL_RGBA : type ) << 26) | (( (uint32)tex->vramAddr >> 3 ) & 0xFFFF );
		} else {
			tex->vramAddr = NULL;
			tex->texFormat = 0;
			return 0;
		}
	}
	else		
		tex->texFormat = (sizeX << 20) | (sizeY << 23) | ((type == GL_RGB ? GL_RGBA : type ) << 26) | ( tex->texFormat & 0xFFFF );
	
	glTexParameter( target, param );
	
	// Copy the texture data into either VRAM or main memory
	if( type != GL_NOTEXTURE && texture ) {
		uint32 vramTemp = vramSetPrimaryBanks(VRAM_A_LCD,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);
		if( type == GL_RGB ) {
			uint16 *src = (uint16*)texture;
			uint16 *dest = (uint16*)tex->vramAddr;
			size >>= 1;
			while (size--) {
				*dest++ = *src | 0x8000;
				src++;
			}
		} else {
			swiCopy( texture, tex->vramAddr, ( size >> 2 ) | COPY_MODE_WORD );
			if( type == GL_COMPRESSED )
				swiCopy( texture + tex->texSize, vramBlock_getAddr( glGlob->vramBlocks[ 0 ], tex->texIndexExt ), ( size >> 3 ) | COPY_MODE_WORD );
		}
		vramRestorePrimaryBanks(vramTemp);		
	}

	return 1;
}


