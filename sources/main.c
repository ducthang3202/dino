// adapted from	the	original acube demo	by tkcne.

// enjoy

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "sprites_tpl.h" 

GXRModeObj	*screenMode;
static void	*xternal_frame_buffer;
static vu8	readyForCopy;
#define	FIFO_SIZE (256*1024)

s16	vertices[] ATTRIBUTE_ALIGN(32) = {
	0, 32, 0,
	-15, -15, 0,
	15,	-15, 0};

u8 colors[]	ATTRIBUTE_ALIGN(32)	= {
	255, 0,	0, 255,		// red
	0, 255,	0, 255,		// green
	0, 0, 255, 255};	// blue

void update_screen();
static void	copy_buffers(u32 unused);

int	main(void) {
	//	Mtx	view;
	//Mtx44	projection;
	GXColor	backgroundColor	= {255, 255, 255,	255};
	void *fifoBuffer = NULL;

	VIDEO_Init();
	WPAD_Init();

	screenMode = VIDEO_GetPreferredMode(NULL);

	xternal_frame_buffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));

	VIDEO_Configure(screenMode);
	VIDEO_SetNextFramebuffer(xternal_frame_buffer);

	// callback after Vsync is called
	VIDEO_SetPostRetraceCallback(copy_buffers);
	VIDEO_SetBlack(false);
	VIDEO_Flush();

	// pipeline for gx
	fifoBuffer = MEM_K0_TO_K1(memalign(32,FIFO_SIZE));
	memset(fifoBuffer,	0, FIFO_SIZE);

	GX_Init(fifoBuffer, FIFO_SIZE);
	GX_SetCopyClear(backgroundColor, 0x00ffffff);

	// set rule for viewport (most cases screen w and h)
	GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);

	// set y scale if external buffer differs from embedded
	GX_SetDispCopyYScale((f32)screenMode->xfbHeight/(f32)screenMode->efbHeight);

	// if viewport is bigger than actual screen it cuts those pixels out
	// enforcing correct viewport if something goes wrong
	GX_SetScissor(0,0,screenMode->fbWidth,screenMode->efbHeight);

	// set copy rectangle when copy buffer to buffer
	GX_SetDispCopySrc(0,0,screenMode->fbWidth,screenMode->efbHeight);
	GX_SetDispCopyDst(screenMode->fbWidth,screenMode->xfbHeight);

	// filter mode for rendering aa = anti aliasing
	GX_SetCopyFilter(screenMode->aa,screenMode->sample_pattern,
					 GX_TRUE,screenMode->vfilter);

	// rendering interlaced mode (one field double the height)
	GX_SetFieldMode(screenMode->field_rendering,
					((screenMode->viHeight==2*screenMode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xternal_frame_buffer,GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	//guVector camera =	{0.0F, 0.0F, 0.0F};
	//guVector up =	{0.0F, 1.0F, 0.0F};
	//guVector look	= {0.0F, 0.0F, -1.0F};
	GXTexObj texObj;
	TPLFile spriteTPL;
	TPL_OpenTPLFromMemory(&spriteTPL, (void *)sprites_tpl, sprites_tpl_size);
	TPL_GetTexture(&spriteTPL,0,&texObj);
	GX_LoadTexObj(&texObj, GX_TEXMAP0);

	Mtx44 ortho;
	guOrtho(ortho, 0, screenMode->fbWidth, 0, screenMode->efbHeight, -1, 1);


	GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);
	// for 3d rendering
	// guPerspective(projection, 60, (CONF_GetAspectRatio() == CONF_ASPECT_16_9) ? 16.0F/9.0F : 4.0F/3.0F, 10.0F,	300.0F);
	// GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT); 

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,	GX_POS_XY, GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8,	0);

    GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_DIRECT, GX_TG_TEX0, GX_IDENTITY);

    //GX_SetNumTevStages(1);
	// Sets the number of color channels active.
	GX_SetNumChans(1);
	// Sets the number of texture coordinate generators.
	GX_SetNumTexGens(0);

	//GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);  

	while (1)
	{
		//guLookAt(view, &camera,	&up, &look);
		//GX_SetViewport(0,0,screenMode->fbWidth,screenMode->efbHeight,0,1);
		GX_InvVtxCache();
		GX_InvalidateTexAll();
		update_screen();

		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) 
			exit(0);
	}
	return 0;
}

void update_screen( )
{
	Mtx modelView;

	guMtxIdentity(modelView);
	GX_LoadPosMtxImm(modelView, GX_PNMTX0);
	/*
	Mtx	modelView;

	guMtxIdentity(modelView);
	guMtxTransApply(modelView, modelView, 0.0F,	0.0F, -50.0F);
	guMtxConcat(viewMatrix,modelView,modelView);

	GX_LoadPosMtxImm(modelView,	GX_PNMTX0);

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);

	GX_Position1x8(0);
	GX_Color1x8(0);
	GX_Position1x8(1);
	GX_Color1x8(1);
	GX_Position1x8(2);
	GX_Color1x8(2);

	GX_End();
*/

	
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

	GX_Position2f32(100.f, 100.f);   // Top-left
	//GX_TexCoord2f32(0, 0);
	GX_Color3f32(1.0f, 0.0f, 0.0f);

	GX_Position2f32(300.f, 100.f);    // Top-right
	//GX_TexCoord2f32(1, 0);
GX_Color3f32(1.0f, 0.0f, 0.0f);
	GX_Position2f32(100.f, 300.f);   // Bottom-right
	//GX_TexCoord2f32(1, 1);
GX_Color3f32(1.0f, 0.0f, 0.0f);
	GX_Position2f32(300.f, 300.f);  // Bottom-left
	//GX_TexCoord2f32(0, 1);
GX_Color3f32(1.0f, 0.0f, 0.0f);
	GX_End();

	GX_DrawDone();
	readyForCopy = GX_TRUE;
	VIDEO_WaitVSync();
	return;
}

static void	copy_buffers(u32 count __attribute__ ((unused)))
{
	if (readyForCopy==GX_TRUE) {
		GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(xternal_frame_buffer,GX_TRUE);
		GX_Flush();
		readyForCopy = GX_FALSE;
	}
}